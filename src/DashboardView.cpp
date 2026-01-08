#include "DashboardView.h"
#include "ui_DashboardView.h"
#include <QFrame>
#include <QGridLayout>
#include "ConfigManager.h"

DashboardView::DashboardView(QWidget *parent)
    : QWidget(parent), 
      ui(new Ui::DashboardView),
      m_metadataClient(new MetadataClient(this)),
      m_coreDataClient(new CoreDataClient(this))
{
    ui->setupUi(this);
    
    m_supportClient = new SupportClient(this);

    // Connections
    connect(m_metadataClient, &MetadataClient::devicesReceived, this, &DashboardView::onDevicesReceived);
    connect(m_metadataClient, &MetadataClient::deviceServicesReceived, this, &DashboardView::onServicesReceived);
    connect(m_metadataClient, &MetadataClient::deviceProfilesReceived, this, &DashboardView::onProfilesReceived);
    connect(m_coreDataClient, &CoreDataClient::eventsReceived, this, &DashboardView::onEventsReceived);
    connect(m_coreDataClient, &CoreDataClient::readingsReceived, this, &DashboardView::onReadingsReceived);
    connect(m_supportClient, &SupportClient::notificationsReceived, this, &DashboardView::onNotificationsReceived);
    connect(m_supportClient, &SupportClient::intervalsReceived, this, &DashboardView::onIntervalsReceived);
    
    connect(ui->refreshButton, &QPushButton::clicked, this, &DashboardView::refresh);

    // Recreate grid items programmatically (simulating dynamic content)
    // We add them to ui->cardGrid
    
    auto createCard = [this](const QString &title, QLabel *&label, QLabel *&subLabel, const QString &color) {
        QFrame *card = new QFrame();
        card->setMinimumSize(250, 120);
        card->setStyleSheet(QString("QFrame { background-color: white; border-radius: 8px; border: 1px solid #dcdde1; }") +
                            "QFrame:hover { border: 1px solid " + color + "; }");
        card->setCursor(Qt::PointingHandCursor);
        
        QVBoxLayout *layout = new QVBoxLayout(card);
        QLabel *titleLabel = new QLabel(title);
        titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #7f8c8d; border: none;");
        
        QHBoxLayout *content = new QHBoxLayout();
        label = new QLabel("0");
        label->setStyleSheet("font-size: 32px; font-weight: bold; color: " + color + "; border: none;");
        
        QVBoxLayout *subInfo = new QVBoxLayout();
        subLabel = new QLabel("");
        subLabel->setStyleSheet("font-size: 11px; color: #95a5a6; border: none;");
        subInfo->addWidget(subLabel);
        subInfo->addStretch();

        content->addWidget(label);
        content->addStretch();
        content->addLayout(subInfo);
        
        layout->addWidget(titleLabel);
        layout->addLayout(content);
        return card;
    };

    ui->cardGrid->addWidget(createCard("Device Services", m_serviceCount, m_serviceLockedCount, "#27ae60"), 0, 0);
    ui->cardGrid->addWidget(createCard("Devices", m_deviceCount, m_deviceLockedCount, "#2980b9"), 0, 1);
    ui->cardGrid->addWidget(createCard("Device Profiles", m_profileCount, m_profileSubLabel, "#e67e22"), 0, 2);
    
    ui->cardGrid->addWidget(createCard("Schedulers", m_schedulerCount, m_schedulerSubLabel, "#34495e"), 1, 0);
    ui->cardGrid->addWidget(createCard("Notifications", m_notificationCount, m_notificationSubLabel, "#e74c3c"), 1, 1);
    ui->cardGrid->addWidget(createCard("Events", m_eventCount, m_eventSubLabel, "#8e44ad"), 1, 2);

    ui->cardGrid->addWidget(createCard("Readings", m_readingCount, m_readingSubLabel, "#f1c40f"), 2, 0);

    refresh();
}

DashboardView::~DashboardView()
{
    delete ui;
}

void DashboardView::refresh()
{
    m_metadataClient->setBaseUrl(ConfigManager::instance().metadataUrl());
    m_metadataClient->fetchDevices();
    m_metadataClient->fetchDeviceServices();
    m_metadataClient->fetchDeviceProfiles();

    m_coreDataClient->setBaseUrl(ConfigManager::instance().coreDataUrl());
    m_coreDataClient->fetchEvents();
    m_coreDataClient->fetchReadings();

    m_supportClient->setBaseUrl(ConfigManager::instance().notificationsUrl());
    m_supportClient->fetchNotifications();
    m_supportClient->setBaseUrl(ConfigManager::instance().schedulerUrl());
    m_supportClient->fetchIntervals();
}

void DashboardView::onDevicesReceived(const QJsonArray &devices)
{
    if(m_deviceCount) m_deviceCount->setText(QString::number(devices.size()));
    int locked = 0;
    for (const auto &v : devices) {
        if (v.toObject()["adminState"].toString() == "LOCKED") locked++;
    }
    if(m_deviceLockedCount) m_deviceLockedCount->setText(QString("Locked: %1\nUnlocked: %2").arg(locked).arg(devices.size() - locked));
}

void DashboardView::onServicesReceived(const QJsonArray &services)
{
    if(m_serviceCount) m_serviceCount->setText(QString::number(services.size()));
    int locked = 0;
    for (const auto &v : services) {
        if (v.toObject()["adminState"].toString() == "LOCKED") locked++;
    }
    if(m_serviceLockedCount) m_serviceLockedCount->setText(QString("Locked: %1\nUnlocked: %2").arg(locked).arg(services.size() - locked));
}

void DashboardView::onProfilesReceived(const QJsonArray &profiles)
{
    if(m_profileCount) m_profileCount->setText(QString::number(profiles.size()));
}

void DashboardView::onEventsReceived(const QJsonArray &events)
{
    if(m_eventCount) m_eventCount->setText(QString::number(events.size()));
}

void DashboardView::onReadingsReceived(const QJsonArray &readings)
{
    if(m_readingCount) m_readingCount->setText(QString::number(readings.size()));
}

void DashboardView::onNotificationsReceived(const QJsonArray &notifications)
{
    if(m_notificationCount) m_notificationCount->setText(QString::number(notifications.size()));
}

void DashboardView::onIntervalsReceived(const QJsonArray &intervals)
{
    if(m_schedulerCount) m_schedulerCount->setText(QString::number(intervals.size()));
}
