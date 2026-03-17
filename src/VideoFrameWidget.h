#ifndef VIDEOFRAMEWIDGET_H
#define VIDEOFRAMEWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QJsonObject>
#include <QJsonArray>

class VideoFrameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoFrameWidget(QWidget *parent = nullptr);

    void setFrame(const QImage &image);
    void setDetections(const QJsonArray &detections);
    void setStatus(const QString &status, const QString &color = "green");
    void setFps(double fps);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage m_currentFrame;
    QJsonArray m_detections;
    QString m_status;
    QString m_statusColor;
    double m_fps;
};

#endif // VIDEOFRAMEWIDGET_H
