#include "SchedulerView.h"
#include "ui_SchedulerView.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QJsonObject>
#include "ConfigManager.h"
#include "AddJsonDialog.h"

SchedulerView::SchedulerView(QWidget *parent)
    : QWidget(parent), ui(new Ui::SchedulerView), m_client(new SupportClient(this))
{
    ui->setupUi(this);
    
    ui->intervalsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->actionsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(m_client, &SupportClient::intervalsReceived, this, &SchedulerView::onIntervalsReceived);
    connect(m_client, &SupportClient::intervalActionsReceived, this, &SchedulerView::onActionsReceived);
    connect(m_client, &SupportClient::operationCompleted, this, &SchedulerView::onOperationCompleted);

    connect(ui->btnRefInt, &QPushButton::clicked, this, &SchedulerView::refresh);
    connect(ui->btnAddInt, &QPushButton::clicked, this, &SchedulerView::onAddInterval);
    connect(ui->btnDelInt, &QPushButton::clicked, this, &SchedulerView::onDeleteInterval);

    connect(ui->btnRefAct, &QPushButton::clicked, this, &SchedulerView::refresh);
    connect(ui->btnAddAct, &QPushButton::clicked, this, &SchedulerView::onAddAction);
    connect(ui->btnDelAct, &QPushButton::clicked, this, &SchedulerView::onDeleteAction);

    refresh();
}

SchedulerView::~SchedulerView()
{
    delete ui;
}

void SchedulerView::refresh()
{
    m_client->setBaseUrl(ConfigManager::instance().schedulerUrl());
    m_client->fetchIntervals();
    m_client->fetchIntervalActions();
}

void SchedulerView::onIntervalsReceived(const QJsonArray &intervals)
{
    ui->intervalsTable->setRowCount(0);
    for (const auto &v : intervals) {
        QJsonObject obj = v.toObject();
        int row = ui->intervalsTable->rowCount();
        ui->intervalsTable->insertRow(row);
        ui->intervalsTable->setItem(row, 0, new QTableWidgetItem(obj["name"].toString()));
        ui->intervalsTable->setItem(row, 1, new QTableWidgetItem(obj["interval"].toString()));
    }
}

void SchedulerView::onActionsReceived(const QJsonArray &actions)
{
    ui->actionsTable->setRowCount(0);
    for (const auto &v : actions) {
        QJsonObject obj = v.toObject();
        int row = ui->actionsTable->rowCount();
        ui->actionsTable->insertRow(row);
        ui->actionsTable->setItem(row, 0, new QTableWidgetItem(obj["name"].toString()));
        ui->actionsTable->setItem(row, 1, new QTableWidgetItem(obj["targetName"].toString()));
        ui->actionsTable->setItem(row, 2, new QTableWidgetItem(obj["intervalName"].toString()));
    }
}

void SchedulerView::onAddInterval()
{
    AddJsonDialog dlg("Add Interval", "{\n  \"apiVersion\": \"v3\",\n  \"interval\": {\n    \"name\": \"new-interval\",\n    \"start\": \"20230101T000000\",\n    \"end\": \"20231231T235959\",\n    \"interval\": \"10s\"\n  }\n}", this);
    if (dlg.exec() == QDialog::Accepted) {
        m_client->setBaseUrl(ConfigManager::instance().schedulerUrl());
        m_client->addInterval(dlg.jsonObject());
    }
}

void SchedulerView::onDeleteInterval()
{
    QTableWidgetItem *item = ui->intervalsTable->currentItem();
    if (!item) return;
    QString name = ui->intervalsTable->item(item->row(), 0)->text();
    if (QMessageBox::question(this, "Confirm", "Delete interval " + name + "?") == QMessageBox::Yes) {
        m_client->setBaseUrl(ConfigManager::instance().schedulerUrl());
        m_client->deleteInterval(name);
    }
}

void SchedulerView::onAddAction()
{
    AddJsonDialog dlg("Add Interval Action", "{\n  \"apiVersion\": \"v3\",\n  \"action\": {\n    \"name\": \"new-action\",\n    \"intervalName\": \"new-interval\",\n    \"address\": {\n      \"type\": \"REST\",\n      \"host\": \"edgex-core-data\",\n      \"port\": 59880,\n      \"path\": \"/api/v3/ping\"\n    }\n  }\n}", this);
    if (dlg.exec() == QDialog::Accepted) {
        m_client->setBaseUrl(ConfigManager::instance().schedulerUrl());
        m_client->addIntervalAction(dlg.jsonObject());
    }
}

void SchedulerView::onDeleteAction()
{
    QTableWidgetItem *item = ui->actionsTable->currentItem();
    if (!item) return;
    QString name = ui->actionsTable->item(item->row(), 0)->text();
    if (QMessageBox::question(this, "Confirm", "Delete interval action " + name + "?") == QMessageBox::Yes) {
        m_client->setBaseUrl(ConfigManager::instance().schedulerUrl());
        m_client->deleteIntervalAction(name);
    }
}

void SchedulerView::onOperationCompleted(bool success, const QString &message)
{
    if (success) {
        QMessageBox::information(this, "Success", message);
        refresh();
    } else {
        QMessageBox::warning(this, "Error", message);
    }
}
