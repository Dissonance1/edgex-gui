#include "LiveMonitoringWidget.h"
#include <QPainter>
#include <QDebug>
#include <QJsonObject>

LiveMonitoringWidget::LiveMonitoringWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setMinimumSize(640, 480);
}

LiveMonitoringWidget::~LiveMonitoringWidget()
{
    stop();
}

void LiveMonitoringWidget::play(const QString &cameraId)
{
    stop();
    m_cameraId = cameraId;
    qDebug() << "LiveMonitoringWidget: Now acting as native Qt renderer for camera:" << cameraId;
}

void LiveMonitoringWidget::stop()
{
    m_cameraId.clear();
    m_currentFrame = QImage();
    m_detections = QJsonArray();
    update();
}

void LiveMonitoringWidget::updateNativeFrame(int streamId, const QImage &frame, const QJsonArray &detections)
{
    m_currentFrame = frame.copy();
    m_detections = detections;
    update();
}

void LiveMonitoringWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    if (!m_currentFrame.isNull()) {
        QImage scaled = m_currentFrame.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int x = (width() - scaled.width()) / 2;
        int y = (height() - scaled.height()) / 2;
        painter.drawImage(x, y, scaled);

        // Draw bounding boxes overlays over the scaled image
        painter.setPen(QPen(Qt::green, 3));
        for (const auto &detRef : m_detections) {
            QJsonObject det = detRef.toObject();
            if (det.contains("box_norm")) {
                QJsonArray boxNorm = det["box_norm"].toArray();
                if (boxNorm.size() == 4) {
                    // Coordinates are normalized [0.0 - 1.0] relative to model input
                    // We map them directly to the scaled image dimensions
                    int bx = x + boxNorm[0].toDouble() * scaled.width();
                    int by = y + boxNorm[1].toDouble() * scaled.height();
                    int bw = boxNorm[2].toDouble() * scaled.width();
                    int bh = boxNorm[3].toDouble() * scaled.height();

                    painter.drawRect(bx, by, bw, bh);

                    // Draw Label with solid background
                    QString labelText = QString("%1: %2%").arg(det["label"].toString()).arg(int(det["score"].toDouble() * 100));
                    QRect textRect = painter.fontMetrics().boundingRect(labelText);
                    textRect.moveTo(bx, by - textRect.height());

                    painter.fillRect(textRect, QColor(0, 255, 0, 150)); // Semi-transparent green background
                    
                    painter.setPen(Qt::black);
                    painter.drawText(textRect, Qt::AlignCenter, labelText);
                    painter.setPen(QPen(Qt::green, 3)); // restore pen
                }
            } else if (det.contains("bounding_box")) {
                // Fallback for old pixel-based detections
                QJsonArray bboxArray = det["bounding_box"].toArray();
                if (bboxArray.size() == 4) {
                    double scaleX = (double)scaled.width() / 640.0; // Assume 640 original
                    double scaleY = (double)scaled.height() / 640.0;

                    int bx = x + bboxArray[0].toInt() * scaleX;
                    int by = y + bboxArray[1].toInt() * scaleY;
                    int bw = (bboxArray[2].toInt() - bboxArray[0].toInt()) * scaleX;
                    int bh = (bboxArray[3].toInt() - bboxArray[1].toInt()) * scaleY;
                    painter.drawRect(bx, by, bw, bh);
                }
            }
        }
    } else {
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, "Waiting for Native Stream...");
    }
}
