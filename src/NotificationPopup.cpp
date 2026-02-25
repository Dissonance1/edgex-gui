#include "NotificationPopup.h"
#include "ui_NotificationPopup.h"
#include "ConfigManager.h"
#include <QDateTime>
#include <QJsonObject>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QScrollArea>

NotificationPopup::NotificationPopup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NotificationPopup),
    m_client(new SupportClient(this)),
    m_currentStatus("NEW")
{
    ui->setupUi(this);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    connect(m_client, &SupportClient::notificationsReceived, this, &NotificationPopup::onNotificationsReceived);
    connect(ui->btnNew, &QPushButton::clicked, [this](){ onTabChanged(0); });
    connect(ui->btnProcessed, &QPushButton::clicked, [this](){ onTabChanged(1); });
    connect(ui->btnEscalated, &QPushButton::clicked, [this](){ onTabChanged(2); });
    connect(ui->btnViewAll, &QPushButton::clicked, this, &NotificationPopup::viewAllRequested);

    // Initial fetch
    fetchNotifications();
}

NotificationPopup::~NotificationPopup()
{
    delete ui;
}

void NotificationPopup::fetchNotifications()
{
    m_client->setBaseUrl(ConfigManager::instance().notificationsUrl());
    m_client->fetchNotificationsByStatus(m_currentStatus, 0, 10);
}

void NotificationPopup::onTabChanged(int index)
{
    if (index == 0) m_currentStatus = "NEW";
    else if (index == 1) m_currentStatus = "PROCESSED";
    else if (index == 2) m_currentStatus = "ESCALATED";
    
    fetchNotifications();
}

void NotificationPopup::onNotificationsReceived(const QJsonArray &notifications)
{
    updateList(notifications);
}

void NotificationPopup::updateList(const QJsonArray &notifications)
{
    // Clear current list
    QLayoutItem *child;
    while ((child = ui->listLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }

    if (notifications.isEmpty()) {
        QLabel *empty = new QLabel("No notifications found");
        empty->setAlignment(Qt::AlignCenter);
        empty->setStyleSheet("color: #95a5a6; padding: 20px;");
        ui->listLayout->addWidget(empty);
        ui->listLayout->addStretch();
        return;
    }

    for (const auto &v : notifications) {
        QJsonObject obj = v.toObject();
        
        QFrame *itemFrame = new QFrame();
        itemFrame->setStyleSheet("QFrame { background: white; border-bottom: 1px solid #F0F0F0; } QFrame:hover { background: #FAFAFA; }");
        QVBoxLayout *itemLayout = new QVBoxLayout(itemFrame);
        itemLayout->setContentsMargins(20, 15, 20, 15);
        itemLayout->setSpacing(8);

        // Header: Tags
        QHBoxLayout *tagLayout = new QHBoxLayout();
        tagLayout->setSpacing(5);
        
        QJsonArray labels = obj["labels"].toArray();
        if (labels.isEmpty()) labels.append(obj["category"].toString());

        for (const auto &l : labels) {
            QLabel *tag = new QLabel(l.toString());
            tag->setStyleSheet("background: #FFF3E0; color: #F9942A; padding: 2px 8px; border-radius: 4px; font-size: 11px; font-weight: bold;");
            tagLayout->addWidget(tag);
        }
        tagLayout->addStretch();
        
        QLabel *timeLabel = new QLabel(formatTimestamp(obj["created"].toVariant().toLongLong()));
        timeLabel->setStyleSheet("color: #BDC3C7; font-size: 11px;");
        tagLayout->addWidget(timeLabel);
        
        itemLayout->addLayout(tagLayout);

        // Content
        QLabel *contentLabel = new QLabel(obj["content"].toString());
        contentLabel->setWordWrap(true);
        contentLabel->setStyleSheet("color: #2C3E50; font-weight: bold; font-size: 13px;");
        itemLayout->addWidget(contentLabel);

        // Footer: Description
        QLabel *descLabel = new QLabel(obj["description"].toString());
        descLabel->setStyleSheet("color: #95A5A6; font-size: 11px;");
        itemLayout->addWidget(descLabel);

        ui->listLayout->addWidget(itemFrame);
    }
    ui->listLayout->addStretch();
}

QString NotificationPopup::formatTimestamp(long long ts)
{
    if (ts == 0) return "";
    if (ts > 1000000000000000LL) ts /= 1000000;
    return QDateTime::fromMSecsSinceEpoch(ts).toLocalTime().toString("HH:mm");
}

void NotificationPopup::focusOutEvent(QFocusEvent *event)
{
    QWidget::focusOutEvent(event);
    hide();
}

bool NotificationPopup::event(QEvent *event)
{
    if (event->type() == QEvent::WindowDeactivate) {
        hide();
    }
    return QWidget::event(event);
}
