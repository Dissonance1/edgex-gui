#include "AIInferenceWorker.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QThread> // Added by user instruction
#include <QHostAddress>

AIInferenceWorker::AIInferenceWorker(const QJsonObject& config, QObject *parent)
    : QObject(parent), m_config(config)
{
    m_profileName = config["profileName"].toString();
    if (m_profileName.isEmpty()) m_profileName = "default";

    m_videoPort = config["videoPort"].toInt();
    m_metaPort  = config["metaPort"].toInt();
    m_cmdPort   = config["cmdPort"].toInt();
    m_ports = { m_videoPort, m_metaPort, m_cmdPort };
    m_aipuCores = config["aipuCores"].toString();

    m_process = new QProcess(this);
    connect(m_process, &QProcess::started, this, &AIInferenceWorker::onProcessStarted);
    connect(m_process, QOverload<QProcess::ProcessError>::of(&QProcess::errorOccurred), this, &AIInferenceWorker::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &AIInferenceWorker::onProcessFinished);

    m_videoSocket = new QTcpSocket(this);
    connect(m_videoSocket, &QTcpSocket::readyRead, this, &AIInferenceWorker::onVideoReadyRead);

    m_metaSocket = new QTcpSocket(this);
    connect(m_metaSocket, &QTcpSocket::readyRead, this, &AIInferenceWorker::onMetaReadyRead);

    m_cmdSocket = new QTcpSocket(this);
    connect(m_cmdSocket, &QTcpSocket::readyRead, this, &AIInferenceWorker::onCmdReadyRead);
}

AIInferenceWorker::~AIInferenceWorker()
{
    stop();
}

void AIInferenceWorker::start()
{
    if (m_process->state() != QProcess::NotRunning) return;

    QString program = "/home/aetina/.cache/axelera/venvs/f07d93d5/bin/python3";
    QStringList arguments;
    arguments << "/data/edgex-gui/axelera_server.py";
    arguments << "--cmd-port"   << QString::number(m_cmdPort);
    arguments << "--video-port" << QString::number(m_videoPort);
    arguments << "--meta-port"  << QString::number(m_metaPort);
    qint64 epoch = QDateTime::currentSecsSinceEpoch();
    arguments << "--log-file"   << QString("/data/edgex-gui/logs/profile_%1_%2.log").arg(m_profileName.toLower().replace(" ", "_")).arg(epoch);
    
    // Pass display mode to backend
    if (m_config["display"].toString() == "Headless") {
        arguments << "--no-display";
    }
    
    // Convert config to JSON for the start command later, but we also pass it if the backend needs initial state
    // For now, the backend will wait for the "start" command on the CMD port.

    emit logMessage(QString("[%1] Launching backend process...").arg(m_profileName));
    
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    
    // CORE PARTITIONING: Extract the starting core from the config string
    // The resource manager usually returns a string like "0", "1", "0,1" etc.
    // We take the first core as the offset.
    QString aipuCoresStr = m_config["aipuCores"].toString();
    QStringList coreList = aipuCoresStr.split(",");
    QString firstCore = coreList.first().trimmed();
    QString coreCount = QString::number(coreList.size() > 0 ? coreList.size() : 1);

    if (!firstCore.isEmpty()) {
        env.insert("AXELERA_CORE_OFFSET", firstCore);
        env.insert("AXELERA_CORE_LIMIT", coreCount);
        emit logMessage(QString("[%1] Pinning to AIPU Core Offset: %2, Limit: %3")
                        .arg(m_profileName, firstCore, coreCount));
    }

    // Axelera SDK Environment for Aetina (1.5.2-1)
    env.insert("AXELERA_FRAMEWORK", "/data/voyager-sdk");
    env.insert("AXELERA_RUNTIME_DIR", "/opt/axelera/runtime-1.5.2-1");
    // Force SDK to look in the global build directory for pre-compiled binaries (.axnet)
    env.insert("AXELERA_BUILD_DIR", "/data/voyager-sdk/build");
    // Force SDK to use the data-partition cache for weights and metadata to skip re-quantization
    env.insert("HOME", "/data/os_data_move/home/aetina");
    env.insert("AXELERA_CACHE_DIR", "/data/os_data_move/home/aetina/.cache/axelera");
    env.insert("LD_LIBRARY_PATH", "/opt/axelera/runtime-1.5.2-1/lib:/data/voyager-sdk/operators/lib");
    env.insert("PYTHONPATH", "/data/voyager-sdk:/opt/axelera/runtime-1.5.2-1/tvm/tvm-src");
    env.insert("PKG_CONFIG_PATH", "/opt/axelera/runtime-1.5.2-1/lib/pkgconfig:/data/voyager-sdk/operators/lib/pkgconfig");
    env.insert("GST_PLUGIN_PATH", "/opt/axelera/runtime-1.5.2-1/lib/gstreamer-1.0:/data/voyager-sdk/operators/lib");
    
    // AIPU Runtime
    env.insert("AIPU_RUNTIME_STAGE0_OMEGA", "/opt/axelera/device-1.5.2-1/omega/bin/start_axelera_runtime_stage0.bin");
    env.insert("AIPU_FIRMWARE_OMEGA", "/opt/axelera/device-1.5.2-1/omega/bin/start_axelera_runtime.elf");
    env.insert("AXELERA_DEVICE_DIR", "/opt/axelera/device-1.5.2-1/omega");
    env.insert("AXELERA_RISCV_TOOLCHAIN_DIR", "/opt/axelera/riscv-gnu-newlib-toolchain-409b951ba662-7");

    // Update PATH to include RISCV toolchain and SDK bins
    QString path = env.value("PATH");
    path = "/opt/axelera/riscv-gnu-newlib-toolchain-409b951ba662-7/bin:" + path;
    path = "/opt/axelera/runtime-1.5.2-1/bin:" + path;
    env.insert("PATH", path);
    
    m_process->setProcessEnvironment(env);
    m_process->start(program, arguments);
}

void AIInferenceWorker::stop()
{
    if (m_process->state() == QProcess::NotRunning) return;

    emit logMessage(QString("[%1] Stopping process...").arg(m_profileName));
    
    // 1. Try clean stop via command socket (if connected)
    if (m_cmdSocket && m_cmdSocket->state() == QAbstractSocket::ConnectedState) {
        m_cmdSocket->write("stop\n");
        m_cmdSocket->flush();
    }

    // 2. Kill process to ensure port release
    if (m_process) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
    
    // 3. Disconnect sockets
    if (m_videoSocket) m_videoSocket->disconnectFromHost();
    if (m_metaSocket) m_metaSocket->disconnectFromHost();
    if (m_cmdSocket) m_cmdSocket->disconnectFromHost();
}

void AIInferenceWorker::setVideoStreamEnabled(bool enabled)
{
    if (!isRunning() || !m_cmdSocket || m_cmdSocket->state() != QAbstractSocket::ConnectedState) {
        return;
    }

    QString cmd = QString("toggle_video:%1\n").arg(enabled ? "on" : "off");
    m_cmdSocket->write(cmd.toUtf8());
    m_cmdSocket->flush();
    
    emit logMessage(QString("[%1] Video Stream: %2").arg(m_profileName, enabled ? "ENABLED" : "DISABLED"));
}

void AIInferenceWorker::onProcessStarted()
{
    emit logMessage(QString("[%1] Backend process started. Connecting sockets...").arg(m_profileName));
    emit statusChanged("Starting");
    
    // Wait a bit for the server to bind ports
    QTimer::singleShot(2000, this, &AIInferenceWorker::connectSockets);
}

void AIInferenceWorker::connectSockets()
{
    qDebug() << QString("[%1] Worker connection attempt %2 for CMD:%3")
                .arg(m_profileName).arg(m_connectRetryCount + 1).arg(m_cmdPort);

    m_cmdSocket->connectToHost("127.0.0.1", m_cmdPort);
    m_videoSocket->connectToHost("127.0.0.1", m_videoPort);
    m_metaSocket->connectToHost("127.0.0.1", m_metaPort);

    auto handleError = [this](const QString& name, QAbstractSocket::SocketError err) {
        qDebug() << QString("[%1] %2 Socket Error: %3").arg(m_profileName, name, m_cmdSocket->errorString());
        
        if (m_connectRetryCount < 3) {
            m_connectRetryCount++;
            qDebug() << QString("[%1] Retrying in 3 seconds...").arg(m_profileName);
            QTimer::singleShot(3000, this, &AIInferenceWorker::connectSockets);
        } else {
            emit logMessage(QString("[%1] %2 Connection FAILED after retries.").arg(m_profileName, name));
        }
    };
    
    connect(m_cmdSocket, &QTcpSocket::errorOccurred, this, [handleError](QAbstractSocket::SocketError err){ handleError("CMD", err); });
    connect(m_videoSocket, &QTcpSocket::errorOccurred, this, [handleError](QAbstractSocket::SocketError err){ handleError("Video", err); });
    connect(m_metaSocket, &QTcpSocket::errorOccurred, this, [handleError](QAbstractSocket::SocketError err){ handleError("Meta", err); });

    // Once command socket is connected, send the 'start' JSON
    connect(m_cmdSocket, &QTcpSocket::connected, this, [this](){
        if (m_config.isEmpty()) return;
        QJsonDocument doc(m_config);
        QString startCmd = "start:" + QString::fromUtf8(doc.toJson(QJsonDocument::Compact)) + "\n";
        m_cmdSocket->write(startCmd.toUtf8());
        
        // Send initial video state if disabled
        if (!m_videoStreamEnabled) {
            QString toggleCmd = "toggle_video:off\n";
            m_cmdSocket->write(toggleCmd.toUtf8());
        }

        m_cmdSocket->flush();
        emit logMessage(QString("[%1] Connected to backend. Configuration sent.").arg(m_profileName));
        emit statusChanged("Running");
    }, Qt::UniqueConnection);
}

void AIInferenceWorker::onVideoReadyRead()
{
    m_videoBuffer += m_videoSocket->readAll();
    while (m_videoBuffer.size() >= 4) {
        // Big Endian [length(4)]
        uint32_t len = ((uint8_t)m_videoBuffer[0] << 24) |
                       ((uint8_t)m_videoBuffer[1] << 16) |
                       ((uint8_t)m_videoBuffer[2] << 8)  |
                       ((uint8_t)m_videoBuffer[3]);
        
        if (len == 0 || len > 10 * 1024 * 1024) {
            m_videoBuffer.remove(0, 4);
            continue;
        }

        if ((uint32_t)m_videoBuffer.size() < 4 + len) break;

        QImage img;
        if (img.loadFromData(reinterpret_cast<const uchar*>(m_videoBuffer.constData() + 4), len, "JPEG")) {
            emit frameReady(img);
        }
        m_videoBuffer.remove(0, 4 + len);
    }
}

void AIInferenceWorker::onMetaReadyRead()
{
    m_metaBuffer += m_metaSocket->readAll();
    while (m_metaBuffer.size() >= 12) {
        // Little Endian: [total_len(4)] [meta_len(4)] [flags(4)] [json...]
        uint32_t totalLen = *reinterpret_cast<const uint32_t*>(m_metaBuffer.constData());
        uint32_t metaLen  = *reinterpret_cast<const uint32_t*>(m_metaBuffer.constData() + 4);

        if (totalLen < 12 || totalLen > 1024 * 1024) {
            m_metaBuffer.remove(0, 4);
            continue;
        }

        if ((uint32_t)m_metaBuffer.size() < totalLen) break;

        QByteArray jsonData = m_metaBuffer.mid(12, metaLen);
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (!doc.isNull()) {
            emit metadataReady(doc.object());
        }
        m_metaBuffer.remove(0, totalLen);
    }
}

void AIInferenceWorker::onCmdReadyRead()
{
    QByteArray data = m_cmdSocket->readAll();
    qDebug() << "[" << m_profileName << "] CMD Response:" << data.trimmed();
}

void AIInferenceWorker::onProcessError(QProcess::ProcessError error)
{
    emit errorOccurred(QString("[%1] Backend process error: %2").arg(m_profileName).arg(error));
    emit statusChanged("Error");
}

void AIInferenceWorker::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    emit logMessage(QString("[%1] Backend execution finished (code %2)").arg(m_profileName).arg(exitCode));
    emit statusChanged("Stopped");
}

QString AIInferenceWorker::profileName() const { return m_profileName; }
bool AIInferenceWorker::isRunning() const { return m_process->state() == QProcess::Running; }
