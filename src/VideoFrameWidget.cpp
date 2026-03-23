#include "VideoFrameWidget.h"
#include <QPainter>
#include <QColor>

VideoFrameWidget::VideoFrameWidget(QWidget *parent)
    : QWidget(parent)
    , m_status("Disconnected")
    , m_statusColor("gray")
    , m_fps(0.0)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setMinimumSize(320, 180);
}

void VideoFrameWidget::setFrame(const QImage &image)
{
    m_currentFrame = image;
    update();
}

void VideoFrameWidget::setDetections(const QJsonArray &detections)
{
    m_detections = detections;
    update();
}

void VideoFrameWidget::setStatus(const QString &status, const QString &color)
{
    m_status = status;
    m_statusColor = color;
    update();
}

void VideoFrameWidget::setFps(double fps)
{
    m_fps = fps;
    update();
}

void VideoFrameWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    
    // Draw background
    painter.fillRect(rect(), Qt::black);

    if (!m_currentFrame.isNull()) {
        QImage scaled = m_currentFrame.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int x = (width() - scaled.width()) / 2;
        int y = (height() - scaled.height()) / 2;
        painter.drawImage(x, y, scaled);

        // Draw Detections (Scaling to current view)
        double scaleX = (double)scaled.width() / m_currentFrame.width();
        double scaleY = (double)scaled.height() / m_currentFrame.height();

        painter.setPen(QPen(Qt::green, 2));
        for (const auto &detRef : m_detections) {
            QJsonObject det = detRef.toObject();
            if (det.contains("bbox")) {
                QJsonArray bbox = det["bbox"].toArray();
                if (bbox.size() == 4) {
                    int bx = x + bbox[0].toInt() * scaleX;
                    int by = y + bbox[1].toInt() * scaleY;
                    int bw = bbox[2].toInt() * scaleX;
                    int bh = bbox[3].toInt() * scaleY;
                    painter.drawRect(bx, by, bw, bh);

                    QString label = det["label"].toString();
                    double conf = det["confidence"].toDouble();
                    painter.drawText(bx, by - 5, QString("%1 (%2%)").arg(label).arg((int)(conf * 100)));
                }
            }
        }
    }

    // Draw Overlay Info
    painter.setPen(Qt::white);
    painter.drawText(10, 20, QString("Status: %1").arg(m_status));
    painter.drawText(10, 40, QString("FPS: %1").arg(m_fps, 0, 'f', 1));
}
