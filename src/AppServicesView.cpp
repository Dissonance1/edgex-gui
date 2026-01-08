#include "AppServicesView.h"
#include "ui_AppServicesView.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QJsonDocument>
#include <QInputDialog>
#include "ConfigManager.h"
#include "AddJsonDialog.h"

AppServicesView::AppServicesView(QWidget *parent)
    : QWidget(parent), ui(new Ui::AppServicesView), m_client(new AppServiceClient(this))
{
    ui->setupUi(this);
    
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(m_client, &AppServiceClient::appServicesReceived, this, &AppServicesView::onAppServicesReceived);
    connect(m_client, &AppServiceClient::configReceived, this, &AppServicesView::onConfigReceived);
    connect(m_client, &AppServiceClient::operationCompleted, this, &AppServicesView::onOperationCompleted);
    
    connect(ui->btnRefresh, &QPushButton::clicked, this, &AppServicesView::refresh);
    connect(ui->btnConfig, &QPushButton::clicked, this, &AppServicesView::onViewConfig);
    connect(ui->btnTemplate, &QPushButton::clicked, this, &AppServicesView::onAddFromTemplate);
    connect(ui->searchEdit, &QLineEdit::textChanged, this, &AppServicesView::onSearch);

    refresh();
}

AppServicesView::~AppServicesView()
{
    delete ui;
}

void AppServicesView::refresh()
{
    m_client->setBaseUrl(ConfigManager::instance().systemUrl());
    m_client->fetchAppServices();
}

void AppServicesView::onAppServicesReceived(const QJsonArray &services)
{
    m_currentServices = services;
    ui->tableWidget->setRowCount(0);
    for (const auto &v : services) {
        QJsonObject obj = v.toObject();
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(obj["serviceId"].toString()));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem("N/A")); // Registry doesn't have states
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem("UP"));  // If it's in registry, it's alive
    }
}

void AppServicesView::onViewConfig()
{
    QModelIndexList selected = ui->tableWidget->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;
    
    int row = selected.first().row();
    QString serviceKey = ui->tableWidget->item(row, 0)->text();
    m_client->setBaseUrl(ConfigManager::instance().systemUrl());
    m_client->fetchServiceConfig(serviceKey);
}

void AppServicesView::onConfigReceived(const QJsonObject &config)
{
    QJsonDocument doc(config);
    AddJsonDialog dlg("App Service Configuration", doc.toJson(), this);
    if (dlg.exec() == QDialog::Accepted) {
        QModelIndexList selected = ui->tableWidget->selectionModel()->selectedRows();
        if (selected.isEmpty()) return;
        QString serviceKey = ui->tableWidget->item(selected.first().row(), 0)->text();
        
        m_client->setBaseUrl(ConfigManager::instance().systemUrl());
        m_client->updateServiceConfig(serviceKey, dlg.jsonObject());
    }
}

void AppServicesView::onOperationCompleted(bool success, const QString &message)
{
    if (success) {
        QMessageBox::information(this, "Success", message);
    } else {
        QMessageBox::warning(this, "Error", message);
    }
}

void AppServicesView::onSearch(const QString &text)
{
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        bool match = false;
        for (int j = 0; j < ui->tableWidget->columnCount(); ++j) {
            if (ui->tableWidget->item(i, j)->text().contains(text, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        ui->tableWidget->setRowHidden(i, !match);
    }
}

void AppServicesView::onAddFromTemplate()
{
    QStringList templates;
    templates << "HTTP Export" << "MQTT Export" << "Functional Pipeline";
    
    bool ok;
    QString item = QInputDialog::getItem(this, "Select Template", "App Service Template:", templates, 0, false, &ok);
    if (!ok || item.isEmpty()) return;
    
    // ... (rest of logic same as before, simplified for this snippet)
    QMessageBox::information(this, "Info", "Template configuration prepared (Simulated).");
}
