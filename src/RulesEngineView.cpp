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
    : QWidget(parent), ui(new Ui::RulesEngineView), m_client(new RulesClient(this))
{
    ui->setupUi(this);
    
    ui->streamsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->rulesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(m_client, &RulesClient::streamsReceived, this, &RulesEngineView::onStreamsReceived);
    connect(m_client, &RulesClient::rulesReceived, this, &RulesEngineView::onRulesReceived);
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
    if (!m_rulesData.contains(id)) return;
    
    QString sql = m_rulesData[id]["sql"].toString();
    AddJsonDialog dlg("Rule SQL Query", sql, this); 
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
