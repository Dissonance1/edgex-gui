#include "AIInferenceWorker.h"
#include <QDebug>
#include <QImage>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTcpSocket>
#include <mutex>

AIInferenceWorker::AIInferenceWorker(const QJsonObject& config, QObject *parent)
    : QThread(parent), m_config(config), m_isRunning(false)
{
}

AIInferenceWorker::~AIInferenceWorker()
{
    stop();
}

static bool sendServerCommand(const QString& host, int port, const QString& cmd, QString& response)
{
    QTcpSocket sock;
    sock.connectToHost(host, port);
    if (!sock.waitForConnected(2000)) {
        return false;
    }
    sock.write(cmd.toUtf8());
    sock.flush();
    if (sock.waitForReadyRead(2000)) {
        response = QString::fromUtf8(sock.readAll()).trimmed();
    }
    sock.disconnectFromHost();
    return true;
}

void AIInferenceWorker::stop()
{
    m_isRunning = false;
    QString response;
    sendServerCommand("127.0.0.1", 5567, "stop", response);
    qDebug() << "[AIInferenceWorker] stop command sent, server responded:" << response;
    wait();
}

void AIInferenceWorker::run()
{
    emit logMessage("Sending start command to Axelera Server...");

    QJsonDocument configDoc(m_config);
    QString startCmd = "start:" + QString::fromUtf8(configDoc.toJson(QJsonDocument::Compact));

    QString response;
    bool started = false;
    for (int i=0; i<5; ++i) {
        if (sendServerCommand("127.0.0.1", 5567, startCmd, response)) {
            if (response == "OK" || response == "ALREADY_RUNNING") {
                started = true;
                break;
            }
        }
        QThread::msleep(1000);
    }

    if (!started) {
        emit errorOccurred("Could not connect to inference server on port 5567.");
        return;
    }

    emit logMessage(QString("Inference started. Model: %1")
        .arg(m_config["modelPath"].toString().section('/', -1)));

    // Connect to metadata (5566) and video (5568)
    QTcpSocket metaSocket;
    metaSocket.connectToHost("127.0.0.1", 5566);
    
    QThread::msleep(1000);
    QTcpSocket videoSocket;
    videoSocket.connectToHost("127.0.0.1", 5568);

    bool metaConnected = metaSocket.waitForConnected(3000);
    bool videoConnected = videoSocket.waitForConnected(3000);

    if (metaConnected) emit logMessage("Metadata stream connected.");
    if (videoConnected) emit logMessage("Video stream connected.");

    m_isRunning = true;
    QJsonArray currentDetections;
    QByteArray videoBuffer;
    QByteArray metaBuffer;

    while (m_isRunning) {
        // --- 1. HANDLE METADATA ---
        if (metaConnected && metaSocket.state() == QAbstractSocket::ConnectedState) {
            if (metaSocket.waitForReadyRead(5)) {
                metaBuffer += metaSocket.readAll();
            }
            
            while (metaBuffer.size() >= 12) {
                // Protocol (Little Endian): [total_len(4)] [meta_len(4)] [flags(4)] [json...]
                uint32_t totalLen = *reinterpret_cast<const uint32_t*>(metaBuffer.constData());
                uint32_t metaLen = *reinterpret_cast<const uint32_t*>(metaBuffer.constData() + 4);
                
                if (totalLen > 10 * 1024 * 1024) { // Safety
                    metaBuffer.clear();
                    break;
                }
                
                if ((uint32_t)metaBuffer.size() < totalLen) break;
                
                QByteArray jsonData = metaBuffer.mid(12, metaLen);
                metaBuffer.remove(0, totalLen);
                
                QJsonDocument doc = QJsonDocument::fromJson(jsonData);
                if (!doc.isNull()) {
                    QJsonObject obj = doc.object();
                    if (obj.contains("detections")) {
                        currentDetections = obj["detections"].toArray();
                    }
                }
            }
        }

        // --- 2. HANDLE VIDEO ---
        if (videoConnected && videoSocket.state() == QAbstractSocket::ConnectedState) {
            if (videoSocket.waitForReadyRead(5)) {
                videoBuffer += videoSocket.readAll();
            }

            QByteArray latestJpeg;
            while (videoBuffer.size() >= 4) {
                // Video Protocol (Big Endian): [length(4)] [JPEG data]
                uint32_t frameLen = ((uint8_t)videoBuffer[0] << 24) |
                                    ((uint8_t)videoBuffer[1] << 16) |
                                    ((uint8_t)videoBuffer[2] << 8)  |
                                    ((uint8_t)videoBuffer[3]);

                if (frameLen == 0 || frameLen > 5 * 1024 * 1024) {
                    videoBuffer.clear();
                    break;
                }

                if ((uint32_t)videoBuffer.size() < 4 + frameLen) break;

                latestJpeg = videoBuffer.mid(4, frameLen);
                videoBuffer.remove(0, 4 + frameLen);
            }

            if (!latestJpeg.isEmpty()) {
                QImage img;
                if (img.loadFromData(latestJpeg, "JPEG")) {
                    emit frameReady(1, img, currentDetections);
                }
            }
        } else {
            // Reconnect attempt if disconnected
            QThread::msleep(100);
        }
        
        // Small yield to prevent 100% CPU usage as we are polling
        if (metaSocket.bytesAvailable() == 0 && videoSocket.bytesAvailable() == 0) {
             QThread::msleep(5);
        }
    }

    metaSocket.disconnectFromHost();
    videoSocket.disconnectFromHost();
    emit logMessage("Inference Pipeline Stopped.");
}
