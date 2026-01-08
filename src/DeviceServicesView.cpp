#include "DeviceServicesView.h"
#include "ui_DeviceServicesView.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QJsonDocument>
#include "ConfigManager.h"
#include "AddJsonDialog.h"

DeviceServicesView::DeviceServicesView(QWidget *parent)
    : QWidget(parent), ui(new Ui::DeviceServicesView), m_model(new DeviceServicesModel(this)), m_client(new MetadataClient(this))
{
    ui->setupUi(this);

    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterKeyColumn(-1);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->tableView->setModel(m_proxyModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // Connections
    connect(m_client, &MetadataClient::deviceServicesReceived, this, &DeviceServicesView::onServicesReceived);
    connect(m_client, &MetadataClient::operationCompleted, this, &DeviceServicesView::onOperationCompleted);
    
    connect(ui->btnRefresh, &QPushButton::clicked, this, &DeviceServicesView::refresh);
    connect(ui->btnEdit, &QPushButton::clicked, this, &DeviceServicesView::onEditService);
    connect(ui->btnDelete, &QPushButton::clicked, this, &DeviceServicesView::onDeleteService);
    connect(ui->btnLock, &QPushButton::clicked, this, &DeviceServicesView::onLockService);
    connect(ui->btnUnlock, &QPushButton::clicked, this, &DeviceServicesView::onUnlockService);
    connect(ui->searchEdit, &QLineEdit::textChanged, this, &DeviceServicesView::onSearch);

    refresh();
}

DeviceServicesView::~DeviceServicesView()
{
    delete ui;
}

void DeviceServicesView::refresh()
{
    m_client->setBaseUrl(ConfigManager::instance().metadataUrl());
    m_client->fetchDeviceServices();
}

void DeviceServicesView::onServicesReceived(const QJsonArray &services)
{
    m_model->setServices(services);
}

void DeviceServicesView::onSearch(const QString &text)
{
    m_proxyModel->setFilterFixedString(text);
}

void DeviceServicesView::onEditService()
{
    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;
    
    QModelIndex sourceIndex = m_proxyModel->mapToSource(selected.first());
    QJsonObject service = m_model->getService(sourceIndex.row());
    
    QJsonDocument doc(service);
    AddJsonDialog dlg("Edit Device Service", doc.toJson(), this);
    if (dlg.exec() == QDialog::Accepted) {
        m_client->setBaseUrl(ConfigManager::instance().metadataUrl());
        m_client->updateDeviceService(dlg.jsonObject());
    }
}

void DeviceServicesView::onLockService()
{
    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;
    
    QModelIndex sourceIndex = m_proxyModel->mapToSource(selected.first());
    QString name = m_model->data(m_model->index(sourceIndex.row(), 0)).toString();
    
    QJsonObject patch;
    patch["apiVersion"] = "v3";
    QJsonObject svc;
    svc["name"] = name;
    svc["adminState"] = "LOCKED";
    patch["service"] = svc;
    
    m_client->setBaseUrl(ConfigManager::instance().metadataUrl());
    m_client->updateDeviceService(patch);
}

void DeviceServicesView::onUnlockService()
{
    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;
    
    QModelIndex sourceIndex = m_proxyModel->mapToSource(selected.first());
    QString name = m_model->data(m_model->index(sourceIndex.row(), 0)).toString();
    
    QJsonObject patch;
    patch["apiVersion"] = "v3";
    QJsonObject svc;
    svc["name"] = name;
    svc["adminState"] = "UNLOCKED";
    patch["service"] = svc;
    
    m_client->setBaseUrl(ConfigManager::instance().metadataUrl());
    m_client->updateDeviceService(patch);
}

void DeviceServicesView::onDeleteService()
{
    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;
    
    QModelIndex sourceIndex = m_proxyModel->mapToSource(selected.first());
    QString name = m_model->data(m_model->index(sourceIndex.row(), 0)).toString(); // Name is col 0
    
    if (QMessageBox::question(this, "Confirm", "Delete service " + name + "?") == QMessageBox::Yes) {
        m_client->setBaseUrl(ConfigManager::instance().metadataUrl());
        m_client->deleteDeviceService(name);
    }
}

void DeviceServicesView::onOperationCompleted(bool success, const QString &message)
{
    if (success) {
        QMessageBox::information(this, "Success", "Operation successful");
        refresh();
    } else {
        QMessageBox::warning(this, "Error", message);
    }
}
