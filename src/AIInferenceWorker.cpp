#include "AIInferenceWorker.h"
#include <QDebug>
#include <QImage>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QProcess>
#include <QFile>
#include <mutex>

AIInferenceWorker::AIInferenceWorker(const QJsonObject& config, QObject *parent)
    : QThread(parent), m_config(config), m_isRunning(false), m_backendLaunched(false)
{
}

AIInferenceWorker::~AIInferenceWorker()
{
    stop();
}

static bool sendServerCommand(const QString& host, int port, const QString& cmd, QString& response, int timeoutMs = 2000)
{
    QTcpSocket sock;
    sock.connectToHost(host, port);
    if (!sock.waitForConnected(timeoutMs)) {
        response = sock.errorString();
        return false;
    }
    sock.write(cmd.toUtf8());
    sock.flush();
    if (sock.waitForReadyRead(timeoutMs)) {
        response = QString::fromUtf8(sock.readAll()).trimmed();
    }
    sock.disconnectFromHost();
    return true;
}

static bool tryAutoLaunchBackend() {
    const QStringList candidates = {
        "/data/edgex-gui/start_backend.sh",
        "/workspaces/edgex-gui/start_backend.sh",
        "./start_backend.sh"
    };

    for (const QString &path : candidates) {
        if (!QFile::exists(path)) continue;

        // make executable if needed
        QFile file(path);
        if (!file.permissions().testFlag(QFile::ExeUser)) {
            QFile::Permissions perms = file.permissions() | QFile::ExeUser;
            file.setPermissions(perms);
        }

        bool ok = QProcess::startDetached("bash", QStringList() << "-c" << QString("bash %1").arg(path));
        if (ok) {
            qDebug() << "[AIInferenceWorker] launched backend script:" << path;
            return true;
        }
    }

    qDebug() << "[AIInferenceWorker] no backend script found to launch.";
    return false;
}

void AIInferenceWorker::stop()
{
    m_isRunning = false;
    QString response;
    if (sendServerCommand("127.0.0.1", 5567, "stop", response, 2000)) {
        qDebug() << "[AIInferenceWorker] stop command sent, server responded:" << response;
    } else {
        qDebug() << "[AIInferenceWorker] stop command failed:" << response;
    }

    if (m_backendLaunched) {
        QString shutdownResp;
        if (sendServerCommand("127.0.0.1", 5567, "shutdown", shutdownResp, 2000)) {
            qDebug() << "[AIInferenceWorker] shutdown command sent, server responded:" << shutdownResp;
        } else {
            qDebug() << "[AIInferenceWorker] shutdown command failed:" << shutdownResp;
        }
        m_backendLaunched = false;
    }

    wait();
}

void AIInferenceWorker::run()
{
    emit logMessage("Sending start command to Axelera Server...");

    QJsonDocument configDoc(m_config);
    QString startCmd = "start:" + QString::fromUtf8(configDoc.toJson(QJsonDocument::Compact));

    QString response;
    bool started = false;
    bool autoLaunched = false;

    for (int i = 0; i < 20; ++i) {
        if (sendServerCommand("127.0.0.1", 5567, startCmd, response, 2000)) {
            if (response == "OK" || response == "ALREADY_RUNNING") {
                started = true;
                break;
            }
            qDebug() << "[AIInferenceWorker] command server replied:" << response;
        } else {
            qDebug() << "[AIInferenceWorker] connect to 5567 failed:" << response;
        }

        if (i == 3 && !autoLaunched) {
            emit logMessage("Port 5567 not reachable, attempting to launch backend script...");
            autoLaunched = tryAutoLaunchBackend();
            if (autoLaunched) {
                emit logMessage("Backend launch requested, waiting for command server to appear...");
            }
        }

        QThread::msleep(1000);
    }

    if (!started) {
        QString hint = "Could not connect to inference server on port 5567. Ensure backend is running (axelera_server).";
        if (!autoLaunched) {
            hint += " Try running start_backend.sh or startup.sh first.";
        }
        emit errorOccurred(hint);
        return;
    }

    m_backendLaunched = autoLaunched;

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
