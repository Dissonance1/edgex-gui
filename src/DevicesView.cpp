#include "DevicesView.h"
#include "ui_DevicesView.h"

#include <QHeaderView>
#include <QMessageBox>
#include "ConfigManager.h"
#include "AddDeviceWizard.h"
#include "CommandExecutionView.h"

DevicesView::DevicesView(QWidget *parent)
    : QWidget(parent), ui(new Ui::DevicesView), m_client(new MetadataClient(this)), m_model(new DevicesTableModel(this))
{
    qDebug() << "DevicesView constructor start";
    ui->setupUi(this);
    
    // Model Setup
    ui->tableView->setModel(m_model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    // Connections
    connect(m_client, &MetadataClient::devicesReceived, this, &DevicesView::onDevicesReceived);
    connect(m_client, &MetadataClient::operationCompleted, this, &DevicesView::onOperationCompleted);
    
    connect(ui->refreshButton, &QPushButton::clicked, this, &DevicesView::refresh);
    connect(ui->addButton, &QPushButton::clicked, this, &DevicesView::onAddDevice);
    // connect(ui->commandButton, &QPushButton::clicked, this, &DevicesView::onViewDevice);
    connect(ui->searchEdit, &QLineEdit::textChanged, this, &DevicesView::onSearch);

    refresh();
    qDebug() << "DevicesView constructor end";
}

DevicesView::~DevicesView()
{
    delete ui;
}

void DevicesView::refresh()
{
    m_client->setBaseUrl(ConfigManager::instance().metadataUrl());
    m_client->fetchDevices();
}

void DevicesView::onDevicesReceived(const QJsonArray &devices)
{
    m_currentDevices = devices;
    m_model->setDevices(devices);
}

void DevicesView::onAddDevice()
{
    AddDeviceWizard wizard(this);
    if (wizard.exec() == QDialog::Accepted) {
        m_client->setBaseUrl(ConfigManager::instance().metadataUrl());
        m_client->addDevice(wizard.deviceData());
    }
}

/*
void DevicesView::onViewDevice()
{
    QModelIndex idx = ui->tableView->currentIndex();
    if (!idx.isValid()) return;
    
    QJsonObject obj = m_model->getDevice(idx.row());
    QString name = obj["name"].toString();
    
    QDialog dlg(this);
    dlg.setWindowTitle("Device Details: " + name);
    dlg.resize(600, 500);
    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    layout->addWidget(new CommandExecutionView(name));
    dlg.exec();
}
*/

void DevicesView::onSearch(const QString &text)
{
    if (text.isEmpty()) {
        m_model->setDevices(m_currentDevices);
        return;
    }
    
    QJsonArray filtered;
    for(const auto &v : m_currentDevices) {
        if(v.toObject()["name"].toString().contains(text, Qt::CaseInsensitive)) {
            filtered.append(v);
        }
    }
    m_model->setDevices(filtered);
}

void DevicesView::onOperationCompleted(bool success, const QString &message)
{
    if (success) {
        refresh();
    } else {
        QMessageBox::warning(this, "Error", message);
    }
}
