#ifndef LIVEMONITORINGWIDGET_H
#define LIVEMONITORINGWIDGET_H

#include <QWidget>
#include <QString>
#include <QImage>
#include <QJsonArray>

// Forward declarations
class LiveMonitoringWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LiveMonitoringWidget(QWidget *parent = nullptr);
    ~LiveMonitoringWidget();

    void play(const QString &cameraId);
    void stop();

    // Native frame injection from AIInferenceWorker thread
    void updateNativeFrame(int streamId, const QImage &frame, const QJsonArray &detections);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_cameraId;
    QImage m_currentFrame;
    QJsonArray m_detections;
};

#endif // LIVEMONITORINGWIDGET_H
