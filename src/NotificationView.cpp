#include "NotificationView.h"
#include "ui_NotificationView.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QJsonObject>
#include "ConfigManager.h"
#include "AddSubscriptionDialog.h"

NotificationView::NotificationView(QWidget *parent)
    : QWidget(parent), ui(new Ui::NotificationView), m_client(new SupportClient(this))
{
    ui->setupUi(this);
    
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->subTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(m_client, &SupportClient::notificationsReceived, this, &NotificationView::onNotificationsReceived);
    connect(m_client, &SupportClient::subscriptionsReceived, this, &NotificationView::onSubscriptionsReceived);
    connect(m_client, &SupportClient::operationCompleted, this, &NotificationView::onOperationCompleted);

    connect(ui->btnRefresh, &QPushButton::clicked, this, &NotificationView::refresh);
    connect(ui->btnCleanup, &QPushButton::clicked, this, &NotificationView::onCleanup);
    
    connect(ui->btnRefreshSub, &QPushButton::clicked, this, &NotificationView::refresh);
    connect(ui->btnAddSub, &QPushButton::clicked, this, &NotificationView::onAddSubscription);
    connect(ui->btnDeleteSub, &QPushButton::clicked, this, &NotificationView::onDeleteSubscription);

    refresh();
}

NotificationView::~NotificationView()
{
    delete ui;
}

void NotificationView::refresh()
{
    m_client->setBaseUrl(ConfigManager::instance().notificationsUrl());
    m_client->fetchNotifications();
    m_client->fetchSubscriptions();
}

void NotificationView::onNotificationsReceived(const QJsonArray &notifications)
{
    ui->table->setRowCount(0);
    for (const auto &v : notifications) {
        QJsonObject obj = v.toObject();
        int row = ui->table->rowCount();
        ui->table->insertRow(row);
        ui->table->setItem(row, 0, new QTableWidgetItem(obj["id"].toString()));
        ui->table->setItem(row, 1, new QTableWidgetItem(obj["category"].toString()));
        ui->table->setItem(row, 2, new QTableWidgetItem(obj["content"].toString()));
        ui->table->setItem(row, 3, new QTableWidgetItem(obj["urgency"].toString()));
        ui->table->setItem(row, 4, new QTableWidgetItem(QString::number(obj["created"].toVariant().toLongLong())));
    }
}

void NotificationView::onSubscriptionsReceived(const QJsonArray &subscriptions)
{
    ui->subTable->setRowCount(0);
    for (const auto &v : subscriptions) {
        QJsonObject obj = v.toObject();
        int row = ui->subTable->rowCount();
        ui->subTable->insertRow(row);
        ui->subTable->setItem(row, 0, new QTableWidgetItem(obj["name"].toString()));
        ui->subTable->setItem(row, 1, new QTableWidgetItem(obj["receiver"].toString()));
        
        QJsonArray cats = obj["categories"].toArray();
        QStringList catList;
        for (const auto &c : cats) catList.append(c.toString());
        ui->subTable->setItem(row, 2, new QTableWidgetItem(catList.join(", ")));

        QJsonArray labels = obj["labels"].toArray();
        QStringList labelList;
        for (const auto &l : labels) labelList.append(l.toString());
        ui->subTable->setItem(row, 3, new QTableWidgetItem(labelList.join(", ")));

        ui->subTable->setItem(row, 4, new QTableWidgetItem(obj["adminState"].toString()));
    }
}

void NotificationView::onAddSubscription()
{
    AddSubscriptionDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        m_client->setBaseUrl(ConfigManager::instance().notificationsUrl());
        m_client->addSubscription(dlg.subscriptionData());
    }
}

void NotificationView::onDeleteSubscription()
{
    QModelIndexList selected = ui->subTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;
    QString name = ui->subTable->item(selected.first().row(), 0)->text();
    if (QMessageBox::question(this, "Confirm", "Delete subscription " + name + "?") == QMessageBox::Yes) {
        m_client->setBaseUrl(ConfigManager::instance().notificationsUrl());
        m_client->deleteSubscription(name);
    }
}

void NotificationView::onDeleteNotification() { }

void NotificationView::onCleanup()
{
    m_client->cleanupNotifications();
}

void NotificationView::onOperationCompleted(bool success, const QString &message)
{
    if (success) {
        QMessageBox::information(this, "Success", message);
        refresh();
    } else {
        QMessageBox::warning(this, "Error", message);
    }
}
