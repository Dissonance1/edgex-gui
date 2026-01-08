#include "SystemStatusView.h"
#include "ui_SystemStatusView.h"
#include <QHeaderView>
#include "ConfigManager.h"

SystemStatusView::SystemStatusView(QWidget *parent)
    : QWidget(parent), ui(new Ui::SystemStatusView), m_model(new ServiceStatusModel(this)), m_client(new SystemClient(this))
{
    ui->setupUi(this);
    
    ui->tableView->setModel(m_model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(m_client, &SystemClient::statusReceived, this, &SystemStatusView::onStatusReceived);
    connect(m_client, &SystemClient::servicesFetched, this, &SystemStatusView::onServicesFetched); // NEW
    connect(ui->btnRefresh, &QPushButton::clicked, this, &SystemStatusView::refresh);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SystemStatusView::refresh);
    m_timer->start(30000); // 30s
    refresh();
}

SystemStatusView::~SystemStatusView()
{
    delete ui;
}

void SystemStatusView::refresh()
{
    // Fetch from Consul instead of hardcoded ping
    m_client->fetchRegisteredServices(ConfigManager::instance().registryUrl());
}

void SystemStatusView::onServicesFetched(const QList<QPair<QString, QString>> &services)
{
    qDebug() << "Discovered" << services.size() << "services from registry";
    m_model->clear();
    for(const auto &s : services) {
        m_model->addService(s.first, s.second);
        // Immediately check status
        m_client->checkServiceStatus(s.first, s.second);
    }
}

void SystemStatusView::onStatusReceived(const QString &name, bool isUp)
{
    m_model->updateStatus(name, isUp);
}
