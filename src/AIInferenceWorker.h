#ifndef AIINFERENCEWORKER_H
#define AIINFERENCEWORKER_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>
#include <QTcpSocket>
#include "InferenceResourceManager.h"

class AIInferenceWorker : public QObject
{
    Q_OBJECT

public:
    explicit AIInferenceWorker(const QJsonObject& config, QObject *parent = nullptr);
    ~AIInferenceWorker();

    void start();
    void stop();
    void setVideoStreamEnabled(bool enabled);
    void setInitialVideoStreamEnabled(bool enabled) { m_videoStreamEnabled = enabled; }
    
    QString profileName() const;
    bool isRunning() const;
    
    PortSet ports() const { return m_ports; }
    QString aipuCores() const { return m_aipuCores; }

signals:
    void frameReady(const QImage &frame);
    void metadataReady(const QJsonObject &meta);
    void statusChanged(const QString &status);
    void errorOccurred(const QString &error);
    void logMessage(const QString &msg);

private slots:
    void onProcessStarted();
    void onProcessError(QProcess::ProcessError error);
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    
    void onVideoReadyRead();
    void onMetaReadyRead();
    void onCmdReadyRead();
    
    void connectSockets();

private:
    QJsonObject m_config;
    QProcess* m_process = nullptr;
    QTcpSocket* m_videoSocket = nullptr;
    QTcpSocket* m_metaSocket = nullptr;
    QTcpSocket* m_cmdSocket = nullptr;
    
    int m_videoPort;
    int m_metaPort;
    int m_cmdPort;
    
    QByteArray m_videoBuffer;
    QByteArray m_metaBuffer;
    
    QString m_profileName;
    PortSet m_ports;
    QString m_aipuCores;
    int m_connectRetryCount = 0;
    bool m_videoStreamEnabled = true;
};

#endif // AIINFERENCEWORKER_H
