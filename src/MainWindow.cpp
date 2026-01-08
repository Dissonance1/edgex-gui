#include "MainWindow.h"
#include "ui_MainWindow.h" // Generated header

#include "DashboardView.h"
#include "DevicesView.h"
#include "DeviceServicesView.h"
#include "ProfilesView.h"
#include "EventsReadingsView.h"
#include "SystemStatusView.h"
#include "NotificationView.h"
// #include "SchedulerView.h"
#include "RulesEngineView.h"
// #include "AppServicesView.h"

#include "SettingsDialog.h"
#include "ConfigManager.h"

#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    qDebug() << "MainWindow constructor start";
    ui->setupUi(this);
    ConfigManager::instance().load();
    
    // Setup Central Stack (Programmatic additions to UI-defined container)
    ui->centralStack->addWidget(new DashboardView());      // 0
    ui->centralStack->addWidget(new DevicesView());        // 1
    ui->centralStack->addWidget(new DeviceServicesView()); // 2
    ui->centralStack->addWidget(new ProfilesView());       // 3
    ui->centralStack->addWidget(new EventsReadingsView()); // 4
    ui->centralStack->addWidget(new SystemStatusView());   // 5
    // ui->centralStack->addWidget(new SchedulerView());      // 6
    ui->centralStack->addWidget(new NotificationView());   // 7
    // ui->centralStack->addWidget(new AppServicesView());    // 8
    ui->centralStack->addWidget(new RulesEngineView());    // 9

    setupNavigation();
    createMenus();
    
    connect(ui->navPanel, &QListWidget::currentRowChanged, this, &MainWindow::onNavItemChanged);
    qDebug() << "MainWindow constructor end";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupNavigation()
{
    // Items are now defined in MainWindow.ui for easier editing in Qt Designer
    ui->navPanel->setCurrentRow(1); // Default to Devices
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
