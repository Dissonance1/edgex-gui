#ifndef RULESCLIENT_H
#define RULESCLIENT_H

#include "EdgexClientBase.h"
#include <QJsonArray>
#include <QJsonObject>

class RulesClient : public EdgexClientBase
{
    Q_OBJECT
public:
    explicit RulesClient(QObject *parent = nullptr);

    void fetchStreams();
    void fetchRules();
    void createStream(const QString &sql);
    void deleteStream(const QString &streamName);
    void createRule(const QString &id, const QString &json);
    void updateRule(const QString &id, const QString &json);
    void deleteRule(const QString &id);
    void startRule(const QString &id);
    void stopRule(const QString &id);
    void fetchRule(const QString &id);
    void fetchRuleStatus(const QString &id);

private:
    void get(const QString &path);

signals:
    void streamsReceived(const QJsonArray &streams);
    void rulesReceived(const QJsonArray &rules);
    void ruleReceived(const QString &id, const QJsonObject &rule);
    void ruleStatusReceived(const QString &id, const QJsonObject &status);
    void operationCompleted(bool success, const QString &message);
};

#endif // RULESCLIENT_H
