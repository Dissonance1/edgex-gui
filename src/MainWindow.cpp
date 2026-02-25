#include "MainWindow.h"
#include "ui_MainWindow.h" // Generated header

#include "DashboardView.h"
#include "DevicesView.h"
#include "DeviceServicesView.h"
#include "ProfilesView.h"
#include "EventsReadingsView.h"
#include "SystemStatusView.h"
#include "NotificationView.h"
#include "RulesEngineView.h"
#include "DataIngestionServer.h"
#include "NotificationToast.h"

#include "SettingsDialog.h"
#include "ConfigManager.h"

#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    qDebug() << "MainWindow constructor start";
    ui->setupUi(this);
    ConfigManager::instance().load();
    // Start Data Ingestion Server (Port 4000)
    m_ingestionServer = new DataIngestionServer(this);
    if (m_ingestionServer->listen(4000)) {
        qInfo() << "EdgeX Ingestion Proxy listening on port 4000";
    } else {
        qWarning() << "Failed to start Ingestion Proxy on port 4000";
    }

    // Set Logo Pixmap explicitly
    // Set Logo Pixmap explicitly
    QPixmap logo(":/resources/honeycomblogo.png");
    if (logo.isNull()) {
        qDebug() << "Failed to load logo from :/resources/honeycomblogo.png";
    } else {
        ui->logoLabel->setPixmap(logo.scaled(200, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        qDebug() << "Logo loaded successfully from resources";
    }
    
    // Setup Central Stack (Programmatic additions to UI-defined container)
    DashboardView *dashView = new DashboardView();
    ui->centralStack->addWidget(dashView);      // 0
    ui->centralStack->addWidget(new DevicesView());        // 1
    ui->centralStack->addWidget(new DeviceServicesView()); // 2
    ui->centralStack->addWidget(new ProfilesView());       // 3
    ui->centralStack->addWidget(new EventsReadingsView()); // 4
    ui->centralStack->addWidget(new SystemStatusView());   // 5
    // ui->centralStack->addWidget(new SchedulerView());      // 6
    NotificationView *notifView = new NotificationView();
    ui->centralStack->addWidget(notifView);
    connect(this, &MainWindow::notificationsUpdated, notifView, &NotificationView::refresh);
    // ui->centralStack->addWidget(new AppServicesView());    // 8
    ui->centralStack->addWidget(new RulesEngineView());    // 9 (Actually 7)

    connect(dashView, &DashboardView::viewAllNotificationsRequested, [this]() {
        // Find "Notifications" in navPanel and select it
        for (int i = 0; i < ui->navPanel->count(); ++i) {
            if (ui->navPanel->item(i)->text() == "Notifications") {
                ui->navPanel->setCurrentRow(i);
                break;
            }
        }
    });

    setupNavigation();
    // createMenus(); // User requested to remove top menus
    
    connect(ui->navPanel, &QListWidget::currentRowChanged, this, &MainWindow::onNavItemChanged);
    connect(ui->btnSettings, &QPushButton::clicked, this, &MainWindow::showSettings);

    // Setup Notification Polling
    m_pollClient = new SupportClient(this);
    QString url = ConfigManager::instance().notificationsUrl();
    qDebug() << "MainWindow: Setting up polling with URL:" << url;
    m_pollClient->setBaseUrl(url);
    connect(m_pollClient, &SupportClient::notificationsReceived, this, &MainWindow::onPollingNotificationsReceived);

    m_pollTimer = new QTimer(this);
    connect(m_pollTimer, &QTimer::timeout, this, [this]() {
        m_pollClient->fetchNotifications(0, 10); // Check for newest alerts regardless of status
    });
    m_pollTimer->start(5000); // Check every 5 seconds

    qDebug() << "MainWindow constructor end";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupNavigation()
{
    // Items are now defined in MainWindow.ui for easier editing in Qt Designer
    ui->navPanel->setCurrentRow(0); // Default to Dashboard (index 0)
    ui->centralStack->setCurrentIndex(0);
}

void MainWindow::createMenus()
{
    // If we had a .ui menu bar we could use it, but keeping this programmatic for now or using the one in .ui
    // The .ui has a menuBar named "menuBar", we can access it via ui->menuBar? or function menuBar()
    // QMainWindow::menuBar() returns the one set.
    
    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Exit", this, &QWidget::close);

    QMenu *toolsMenu = menuBar()->addMenu("&Tools");
    toolsMenu->addAction("&Settings...", this, &MainWindow::showSettings);
    toolsMenu->addAction("&Test Connection", this, &MainWindow::testConnection);

    QMenu *helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About", this, [this](){});
}

void MainWindow::onNavItemChanged(int index)
{
    QListWidgetItem *item = ui->navPanel->item(index);
    if (!item) return;

    QString text = item->text();
    if (text == "Dashboard") ui->centralStack->setCurrentIndex(0);
    else if (text == "Devices") ui->centralStack->setCurrentIndex(1);
    else if (text == "Device Services") ui->centralStack->setCurrentIndex(2);
    else if (text == "Device Profiles") ui->centralStack->setCurrentIndex(3);
    else if (text == "Events & Readings") ui->centralStack->setCurrentIndex(4);
    else if (text == "System Status") ui->centralStack->setCurrentIndex(5);
    // else if (text == "Scheduler") ui->centralStack->setCurrentIndex(6);
    else if (text == "Notifications") ui->centralStack->setCurrentIndex(6);
    // else if (text == "App Services") ui->centralStack->setCurrentIndex(8);
    else if (text == "Rules Engine") ui->centralStack->setCurrentIndex(7);
}

void MainWindow::showSettings()
{
    SettingsDialog dlg(this);
    dlg.exec(); // Modal
}

void MainWindow::testConnection()
{
    // Placeholder
}

void MainWindow::onPollingNotificationsReceived(const QJsonArray &notifications)
{
    static bool firstRun = true;
    qDebug() << "MainWindow Poll Received:" << notifications.size() << "notifications";
    if (notifications.isEmpty()) return;

    QJsonObject latest = notifications.first().toObject();
    QString currentId = latest["id"].toString();

    if (firstRun || m_lastNotificationId.isEmpty()) {
        qDebug() << "Initial notification record set to:" << currentId;
        m_lastNotificationId = currentId;
        firstRun = false;
        
        // Refresh the UI on first poll if there are notifications
        emit notificationsUpdated();
        return;
    }

    if (currentId != m_lastNotificationId) {
        qDebug() << "New notification detected!" << currentId << "Old:" << m_lastNotificationId;
        m_lastNotificationId = currentId;

        // Show Toast
        QString category = latest["category"].toString();
        QString description = latest["description"].toString();
        if (description.isEmpty()) description = latest["content"].toString();
        
        NotificationToast::showToast(category, description, this);
        
        // Signal NotificationView to refresh automatically
        emit notificationsUpdated();
    }
}
