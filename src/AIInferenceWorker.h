#ifndef AIINFERENCEWORKER_H
#define AIINFERENCEWORKER_H

#include <QThread>
#include <QImage>
#include <QString>
#include <QJsonArray>
#include <QJsonObject>
#include <atomic>

class AIInferenceWorker : public QThread
{
    Q_OBJECT

public:
    explicit AIInferenceWorker(const QJsonObject& config, QObject *parent = nullptr);
    ~AIInferenceWorker();

    void stop();

signals:
    void frameReady(int streamId, const QImage &frame, const QJsonArray &detections);
    void errorOccurred(const QString &error);
    void logMessage(const QString &msg);

protected:
    void run() override;

private:
    QJsonObject m_config;
    std::atomic<bool> m_isRunning;
    bool m_backendLaunched;
};

#endif // AIINFERENCEWORKER_H
