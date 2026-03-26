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
    ui->detailTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->table, &QTableWidget::itemClicked, this, &NotificationView::onNotificationSelected);

    connect(m_client, &SupportClient::notificationsReceived, this, &NotificationView::onNotificationsReceived);
    connect(m_client, &SupportClient::subscriptionsReceived, this, &NotificationView::onSubscriptionsReceived);
    connect(m_client, &SupportClient::operationCompleted, this, &NotificationView::onOperationCompleted);

    connect(ui->btnRefresh, &QPushButton::clicked, this, &NotificationView::refresh);
    connect(ui->btnCleanup, &QPushButton::clicked, this, &NotificationView::onCleanup);
    connect(ui->btnCleanupByAge, &QPushButton::clicked, this, &NotificationView::onCleanupByAge);
    connect(ui->btnSearch, &QPushButton::clicked, this, &NotificationView::onSearch);
    connect(ui->comboStatus, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &NotificationView::onStatusChanged);
    
    ui->dateStart->setDateTime(QDateTime::currentDateTime().addDays(-1));
    ui->dateEnd->setDateTime(QDateTime::currentDateTime());

    connect(ui->btnToggleAdvanced, &QPushButton::toggled, this, &NotificationView::onToggleAdvanced);
    
    // Hide advanced options by default
    ui->dateStart->hide();
    ui->dateEnd->hide();
    ui->btnSearch->hide();
    ui->label->hide();
    ui->editCleanupAge->hide();
    ui->btnCleanupByAge->hide();
    ui->btnCleanup->hide();

    connect(ui->btnRefreshSub, &QPushButton::clicked, this, &NotificationView::refresh);
    connect(ui->btnAddSub, &QPushButton::clicked, this, &NotificationView::onAddSubscription);
    connect(ui->btnEditSub, &QPushButton::clicked, this, &NotificationView::onEditSubscription);
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
    m_client->fetchNotifications(0, 200); // Fetch more by default
    m_client->fetchSubscriptions();
}

void NotificationView::onToggleAdvanced(bool checked)
{
    ui->dateStart->setVisible(checked);
    ui->dateEnd->setVisible(checked);
    ui->btnSearch->setVisible(checked);
    ui->label->setVisible(checked);
    ui->editCleanupAge->setVisible(checked);
    ui->btnCleanupByAge->setVisible(checked);
    ui->btnCleanup->setVisible(checked);
}

void NotificationView::onNotificationsReceived(const QJsonArray &notifications)
{
    qDebug() << "NotificationView: Received" << notifications.size() << "notifications";
    m_lastNotifications = notifications;
    ui->table->setRowCount(0);
    for (const auto &v : notifications) {
        QJsonObject obj = v.toObject();
        int row = ui->table->rowCount();
        ui->table->insertRow(row);
        
        auto createStaticItem = [](const QString &text) {
            QTableWidgetItem *item = new QTableWidgetItem(text);
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable); // Not editable
            return item;
        };

        ui->table->setItem(row, 0, createStaticItem(obj["id"].toString()));
        ui->table->setItem(row, 1, createStaticItem(obj["category"].toString()));
        
        QJsonArray labelsArr = obj["labels"].toArray();
        QStringList labels;
        for (const auto &l : labelsArr) labels.append(l.toString());
        ui->table->setItem(row, 2, createStaticItem(labels.join(", ")));
        
        ui->table->setItem(row, 3, createStaticItem(obj["content"].toString()));
        ui->table->setItem(row, 4, createStaticItem(obj["contentType"].toString()));
        ui->table->setItem(row, 5, createStaticItem(obj["description"].toString()));
        ui->table->setItem(row, 6, createStaticItem(obj["sender"].toString()));
        ui->table->setItem(row, 7, createStaticItem(obj["severity"].toString()));
        ui->table->setItem(row, 8, createStaticItem(obj["status"].toString()));
        
        long long created = obj["created"].toVariant().toLongLong();
        long long modified = obj["modified"].toVariant().toLongLong();
        
        auto formatTimestamp = [](long long ts) {
            if (ts == 0) return QString("N/A");
            // If > 1e15, it's likely nanoseconds, so convert to milliseconds
            if (ts > 1000000000000000LL) ts /= 1000000;
            return QDateTime::fromMSecsSinceEpoch(ts).toLocalTime().toString("yyyy-MM-dd HH:mm:ss");
        };

        ui->table->setItem(row, 9, createStaticItem(formatTimestamp(created)));
        ui->table->setItem(row, 10, createStaticItem(formatTimestamp(modified)));
    }
}

void NotificationView::onSubscriptionsReceived(const QJsonArray &subscriptions)
{
    m_lastSubscriptions = subscriptions;
    ui->subTable->setRowCount(0);
    for (const auto &v : subscriptions) {
        QJsonObject obj = v.toObject();
        int row = ui->subTable->rowCount();
        ui->subTable->insertRow(row);

        auto createStaticItem = [](const QString &text) {
            QTableWidgetItem *item = new QTableWidgetItem(text);
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            return item;
        };

        ui->subTable->setItem(row, 0, createStaticItem(obj["name"].toString()));
        ui->subTable->setItem(row, 1, createStaticItem(obj["description"].toString()));
        
        QJsonArray channels = obj["channels"].toArray();
        QStringList chanList;
        for (const auto &v_chan : channels) {
            QJsonObject chan = v_chan.toObject();
            chanList.append(chan["type"].toString());
        }
        ui->subTable->setItem(row, 2, createStaticItem(chanList.join(", ")));
        
        QJsonArray cats = obj["categories"].toArray();
        QStringList catList;
        for (const auto &c : cats) catList.append(c.toString());
        ui->subTable->setItem(row, 3, createStaticItem(catList.join(", ")));

        QJsonArray labels = obj["labels"].toArray();
        QStringList labelList;
        for (const auto &l : labels) labelList.append(l.toString());
        ui->subTable->setItem(row, 4, createStaticItem(labelList.join(", ")));

        ui->subTable->setItem(row, 5, createStaticItem(obj["adminState"].toString()));
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

void NotificationView::onEditSubscription()
{
    QModelIndexList selected = ui->subTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "Edit", "Please select a subscription to edit.");
        return;
    }
    
    int row = selected.first().row();
    if (row < 0 || row >= m_lastSubscriptions.size()) return;

    QJsonObject subObj = m_lastSubscriptions[row].toObject();
    AddSubscriptionDialog dlg(this);
    dlg.setSubscription(subObj);
    
    if (dlg.exec() == QDialog::Accepted) {
        m_client->setBaseUrl(ConfigManager::instance().notificationsUrl());
        m_client->updateSubscription(dlg.subscriptionData());
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

void NotificationView::onCleanupByAge()
{
    bool ok;
    long long age = ui->editCleanupAge->text().toLongLong(&ok);
    if (ok) {
        m_client->cleanupNotificationsByAge(age);
    } else {
        QMessageBox::warning(this, "Error", "Invalid age value. Please enter numeric nanoseconds.");
    }
}

void NotificationView::onSearch()
{
    // Search now uses milliseconds for V3 Support Notifications
    long long start = ui->dateStart->dateTime().toMSecsSinceEpoch();
    long long end = ui->dateEnd->dateTime().toMSecsSinceEpoch();
    m_client->fetchNotificationsByTimeRange(start, end);
}

void NotificationView::onStatusChanged(int index)
{
    QString status = ui->comboStatus->currentText();
    m_client->fetchNotificationsByStatus(status);
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

void NotificationView::onNotificationSelected(QTableWidgetItem *item)
{
    if (!item) return;
    int row = item->row();
    if (row < 0 || row >= m_lastNotifications.size()) return;

    QJsonObject obj = m_lastNotifications[row].toObject();
    ui->detailTable->setRowCount(0);

    auto addDetail = [this](const QString &field, const QString &value) {
        int r = ui->detailTable->rowCount();
        ui->detailTable->insertRow(r);
        QTableWidgetItem *fItem = new QTableWidgetItem(field);
        QTableWidgetItem *vItem = new QTableWidgetItem(value);
        fItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        vItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui->detailTable->setItem(r, 0, fItem);
        ui->detailTable->setItem(r, 1, vItem);
    };

    QStringList keys = obj.keys();
    for (const QString &key : keys) {
        QJsonValue val = obj.value(key);
        QString displayVal;
        if (val.isArray()) {
            QJsonDocument doc(val.toArray());
            displayVal = doc.toJson(QJsonDocument::Compact);
        } else if (val.isObject()) {
            QJsonDocument doc(val.toObject());
            displayVal = doc.toJson(QJsonDocument::Compact);
        } else if (key == "created" || key == "modified") {
            long long ts = val.toVariant().toLongLong();
            if (ts > 1000000000000000LL) ts /= 1000000;
            displayVal = QDateTime::fromMSecsSinceEpoch(ts).toLocalTime().toString("yyyy-MM-dd HH:mm:ss");
        } else {
            displayVal = val.toVariant().toString();
        }
        addDetail(key, displayVal);
    }
}
