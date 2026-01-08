#include "RulesClient.h"
#include <QJsonDocument>
#include <QJsonObject>

RulesClient::RulesClient(QObject *parent)
    : EdgexClientBase(parent)
{
}

void RulesClient::fetchStreams()
{
    get("streams"); // eKuiper streams
}

void RulesClient::fetchRules()
{
    get("rules"); // eKuiper rules
}

void RulesClient::createStream(const QString &sql)
{
    QJsonObject obj;
    obj["sql"] = sql;
    post("streams", obj, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Stream created successfully" : error);
    });
}

void RulesClient::deleteStream(const QString &streamName)
{
    deleteRequest("streams/" + streamName, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Stream deleted successfully" : error);
    });
}

void RulesClient::createRule(const QString &id, const QString &json)
{
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject obj = doc.object();
    obj["id"] = id;
    
    post("rules", obj, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Rule created successfully" : error);
    });
}

void RulesClient::deleteRule(const QString &id)
{
    deleteRequest("rules/" + id, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Rule deleted successfully" : error);
    });
}

void RulesClient::startRule(const QString &id)
{
    post("rules/" + id + "/start", QJsonObject(), [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Rule started" : error);
    });
}

void RulesClient::stopRule(const QString &id)
{
    post("rules/" + id + "/stop", QJsonObject(), [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Rule stopped" : error);
    });
}

void RulesClient::fetchRuleStatus(const QString &id)
{
    QString path = "rules/" + id + "/status";
    get(path);
}

void RulesClient::get(const QString &path)
{
    QString fullPath = baseUrl();
    if (!fullPath.endsWith('/') && !path.startsWith('/')) fullPath += "/";
    
    QNetworkRequest request(QUrl(fullPath + path));
    QNetworkReply *reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, path]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (path == "streams") emit streamsReceived(doc.array());
            else if (path == "rules") emit rulesReceived(doc.array());
            else if (path.startsWith("rules/") && path.endsWith("/status")) {
                QString id = path.section('/', 1, 1);
                emit ruleStatusReceived(id, doc.object());
            }
        }
        reply->deleteLater();
    });
}
