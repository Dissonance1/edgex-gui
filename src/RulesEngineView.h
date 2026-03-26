#ifndef RULESENGINEVIEW_H
#define RULESENGINEVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include <QPushButton>
#include "RulesClient.h"

namespace Ui {
class RulesEngineView;
}

class RulesEngineView : public QWidget
{
    Q_OBJECT
public:
    explicit RulesEngineView(QWidget *parent = nullptr);
    ~RulesEngineView();

private slots:
    void refresh();
    void onStreamsReceived(const QJsonArray &streams);
    void onStreamReceived(const QString &streamName, const QJsonObject &stream);
    void onRulesReceived(const QJsonArray &rules);
    void onRuleReceived(const QString &id, const QJsonObject &rule);
    void onRuleStatusReceived(const QString &id, const QJsonObject &status);
    void onOperationCompleted(bool success, const QString &message);
    
    void onAddStream();
    void onEditStream();
    void onDeleteStream();
    void onAddRule();
    void onEditRule();
    void onDeleteRule();
    void onStartRule();
    void onStopRule();
    void onViewSql();
    void onViewMetrics();

private:
    Ui::RulesEngineView *ui;
    RulesClient *m_client;
    QPushButton *m_btnEditStream; 
    QMap<QString, QJsonObject> m_rulesData;
    QString m_pendingRuleId;
    QString m_pendingStreamName;
    bool m_isEditing;
    bool m_isViewingSql;
};

#endif // RULESENGINEVIEW_H
