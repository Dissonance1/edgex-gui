#ifndef RULESENGINEVIEW_H
#define RULESENGINEVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
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
    void onRulesReceived(const QJsonArray &rules);
    void onRuleStatusReceived(const QString &id, const QJsonObject &status);
    void onOperationCompleted(bool success, const QString &message);
    
    void onAddStream();
    void onDeleteStream();
    void onAddRule();
    void onDeleteRule();
    void onStartRule();
    void onStopRule();
    void onViewSql();
    void onViewMetrics();

private:
    Ui::RulesEngineView *ui;
    RulesClient *m_client;
    QMap<QString, QJsonObject> m_rulesData;
};

#endif // RULESENGINEVIEW_H
