#include "RulesEngineView.h"
#include "ui_RulesEngineView.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QJsonDocument>
#include "ConfigManager.h"
#include "AddStreamDialog.h"
#include "AddRuleDialog.h"
#include "AddJsonDialog.h"

RulesEngineView::RulesEngineView(QWidget *parent)
    : QWidget(parent), ui(new Ui::RulesEngineView), m_client(new RulesClient(this)), m_isEditing(false), m_isViewingSql(false)
{
    ui->setupUi(this);
    
    ui->streamsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->rulesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->rulesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->streamsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(m_client, &RulesClient::streamsReceived, this, &RulesEngineView::onStreamsReceived);
    connect(m_client, &RulesClient::rulesReceived, this, &RulesEngineView::onRulesReceived);
    connect(m_client, &RulesClient::ruleReceived, this, &RulesEngineView::onRuleReceived);
    connect(m_client, &RulesClient::ruleStatusReceived, this, &RulesEngineView::onRuleStatusReceived);
    connect(m_client, &RulesClient::operationCompleted, this, &RulesEngineView::onOperationCompleted);
    
    connect(ui->btnRefreshStreams, &QPushButton::clicked, this, &RulesEngineView::refresh);
    connect(ui->btnAddStream, &QPushButton::clicked, this, &RulesEngineView::onAddStream);
    connect(ui->btnDeleteStream, &QPushButton::clicked, this, &RulesEngineView::onDeleteStream);

    connect(ui->btnRefreshRules, &QPushButton::clicked, this, &RulesEngineView::refresh);
    connect(ui->btnAddRule, &QPushButton::clicked, this, &RulesEngineView::onAddRule);
    connect(ui->btnStartRule, &QPushButton::clicked, this, &RulesEngineView::onStartRule);
    connect(ui->btnStopRule, &QPushButton::clicked, this, &RulesEngineView::onStopRule);
    connect(ui->btnViewSql, &QPushButton::clicked, this, &RulesEngineView::onViewSql);
    connect(ui->btnMetrics, &QPushButton::clicked, this, &RulesEngineView::onViewMetrics);
    connect(ui->btnDeleteRule, &QPushButton::clicked, this, &RulesEngineView::onDeleteRule);
    connect(ui->btnEditRule, &QPushButton::clicked, this, &RulesEngineView::onEditRule);

    refresh();
}

RulesEngineView::~RulesEngineView()
{
    delete ui;
}

void RulesEngineView::refresh()
{
    m_client->setBaseUrl(ConfigManager::instance().rulesUrl());
    m_client->fetchStreams();
    m_client->fetchRules();
}

void RulesEngineView::onStreamsReceived(const QJsonArray &streams)
{
    ui->streamsTable->setRowCount(0);
    for (const auto &v : streams) {
        int row = ui->streamsTable->rowCount();
        ui->streamsTable->insertRow(row);
        ui->streamsTable->setItem(row, 0, new QTableWidgetItem(v.toString()));
    }
}

void RulesEngineView::onRulesReceived(const QJsonArray &rules)
{
    m_rulesData.clear();
    ui->rulesTable->setRowCount(0);
    for (const auto &v : rules) {
        QJsonObject obj = v.toObject();
        QString id = obj["id"].toString();
        m_rulesData[id] = obj;

        int row = ui->rulesTable->rowCount();
        ui->rulesTable->insertRow(row);
        ui->rulesTable->setItem(row, 0, new QTableWidgetItem(id));
        ui->rulesTable->setItem(row, 1, new QTableWidgetItem(obj["sql"].toString()));
        
        QJsonArray actions = obj["actions"].toArray();
        QStringList actList;
        for (const auto &a : actions) actList.append(a.toObject().keys().join(","));
        ui->rulesTable->setItem(row, 2, new QTableWidgetItem(actList.join(" | ")));
        ui->rulesTable->setItem(row, 3, new QTableWidgetItem(obj["status"].toString()));
        ui->rulesTable->setItem(row, 4, new QTableWidgetItem("Select to load..."));
    }
}

void RulesEngineView::onRuleStatusReceived(const QString &id, const QJsonObject &status)
{
    for (int i = 0; i < ui->rulesTable->rowCount(); ++i) {
        if (ui->rulesTable->item(i, 0)->text() == id) {
            QString summary = QString("In: %1 | Out: %2 | Err: %3")
                .arg(status["source_messages_count"].toVariant().toLongLong())
                .arg(status["sink_messages_count"].toVariant().toLongLong())
                .arg(status["source_exceptions_count"].toVariant().toLongLong());
            ui->rulesTable->setItem(i, 4, new QTableWidgetItem(summary));
            break;
        }
    }
}

void RulesEngineView::onRuleReceived(const QString &id, const QJsonObject &rule)
{
    m_rulesData[id] = rule;
    
    // Update table with SQL if it was missing
    for (int i = 0; i < ui->rulesTable->rowCount(); ++i) {
        if (ui->rulesTable->item(i, 0)->text() == id) {
            ui->rulesTable->setItem(i, 1, new QTableWidgetItem(rule["sql"].toString()));
            break;
        }
    }

    if (id == m_pendingRuleId) {
        if (m_isEditing) {
            m_isEditing = false;
            m_pendingRuleId = "";
            onEditRule(); // Try again now that we have data
        } else if (m_isViewingSql) {
            m_isViewingSql = false;
            m_pendingRuleId = "";
            onViewSql(); // Try again now that we have data
        }
    }
}

void RulesEngineView::onAddStream()
{
    AddStreamDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        m_client->createStream(dlg.sql());
    }
}

void RulesEngineView::onDeleteStream()
{
    QTableWidgetItem *item = ui->streamsTable->currentItem();
    if (!item) return;
    QString name = item->text();
    if (QMessageBox::question(this, "Confirm", "Delete stream " + name + "?") == QMessageBox::Yes) {
        m_client->deleteStream(name);
    }
}

void RulesEngineView::onAddRule()
{
    AddRuleDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        m_client->setBaseUrl(ConfigManager::instance().rulesUrl());
        QJsonObject rule = dlg.ruleData();
        QString json = QJsonDocument(rule).toJson(QJsonDocument::Compact);
        m_client->createRule(dlg.id(), json);
    }
}

void RulesEngineView::onEditRule()
{
    QModelIndexList selected = ui->rulesTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;
    QString id = ui->rulesTable->item(selected.first().row(), 0)->text();
    
    // Check if we have the full rule (SQL)
    if (!m_rulesData.contains(id) || m_rulesData[id]["sql"].toString().isEmpty()) {
        m_pendingRuleId = id;
        m_isEditing = true;
        m_client->fetchRule(id);
        return;
    }

    AddRuleDialog dlg(this);
    dlg.setRuleData(id, m_rulesData[id]);
    
    if (dlg.exec() == QDialog::Accepted) {
        m_client->setBaseUrl(ConfigManager::instance().rulesUrl());
        QJsonObject rule = dlg.ruleData();
        QString json = QJsonDocument(rule).toJson(QJsonDocument::Compact);
        
        // eKuiper edit uses PUT /rules/{id}
        m_client->updateRule(id, json); 
    }
}

void RulesEngineView::onDeleteRule()
{
    QModelIndexList selected = ui->rulesTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;
    QString id = ui->rulesTable->item(selected.first().row(), 0)->text();
    if (QMessageBox::question(this, "Confirm", "Delete rule " + id + "?") == QMessageBox::Yes) {
        m_client->deleteRule(id);
    }
}

void RulesEngineView::onStartRule()
{
    QModelIndexList selected = ui->rulesTable->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;
    QString id = ui->rulesTable->item(selected.first().row(), 0)->text();
    m_client->startRule(id);
}

void RulesEngineView::onStopRule()
{
    QTableWidgetItem *item = ui->rulesTable->currentItem();
    if (!item) return;
    m_client->setBaseUrl(ConfigManager::instance().rulesUrl());
    m_client->stopRule(ui->rulesTable->item(item->row(), 0)->text());
}

void RulesEngineView::onViewSql()
{
    QTableWidgetItem *item = ui->rulesTable->currentItem();
    if (!item) return;
    QString id = ui->rulesTable->item(item->row(), 0)->text();
    
    if (!m_rulesData.contains(id) || m_rulesData[id]["sql"].toString().isEmpty()) {
        m_pendingRuleId = id;
        m_isViewingSql = true;
        m_client->fetchRule(id);
        return;
    }
    
    QString sql = m_rulesData[id]["sql"].toString();
    AddJsonDialog dlg("Rule SQL Query", sql, this, false); // false = no JSON validation
    dlg.exec();
}

void RulesEngineView::onViewMetrics()
{
    QTableWidgetItem *item = ui->rulesTable->currentItem();
    if (!item) return;
    QString id = ui->rulesTable->item(item->row(), 0)->text();
    m_client->setBaseUrl(ConfigManager::instance().rulesUrl());
    m_client->fetchRuleStatus(id);
}

void RulesEngineView::onOperationCompleted(bool success, const QString &message)
{
    if (success) {
        QMessageBox::information(this, "Success", message);
        refresh();
    } else {
        QMessageBox::warning(this, "Error", message);
    }
}
