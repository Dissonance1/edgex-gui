#include "SupportClient.h"
#include <QJsonDocument>
#include <QJsonObject>

SupportClient::SupportClient(QObject *parent)
    : EdgexClientBase(parent)
{
}

void SupportClient::fetchNotifications()
{
    get("api/v3/notification/all");
}

void SupportClient::deleteNotification(const QString &id)
{
    deleteRequest("api/v3/notification/id/" + id, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Notification deleted" : error);
    });
}

void SupportClient::cleanupNotifications()
{
    // Custom cleanup logic if available, or delete one by one
}

void SupportClient::fetchSubscriptions()
{
    get("api/v3/subscription/all");
}

void SupportClient::addSubscription(const QJsonObject &subscription)
{
    QJsonArray array;
    array.append(subscription);
    post("api/v3/subscription", array, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Subscription added" : error);
    });
}

void SupportClient::deleteSubscription(const QString &name)
{
    deleteRequest("api/v3/subscription/name/" + name, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Subscription deleted" : error);
    });
}

void SupportClient::fetchIntervals()
{
    get("api/v3/interval/all");
}

void SupportClient::addInterval(const QJsonObject &interval)
{
    QJsonArray array;
    array.append(interval);
    post("api/v3/interval", array, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Interval added" : error);
    });
}

void SupportClient::deleteInterval(const QString &name)
{
    deleteRequest("api/v3/interval/name/" + name, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Interval deleted" : error);
    });
}

void SupportClient::fetchIntervalActions()
{
    get("api/v3/intervalaction/all");
}

void SupportClient::addIntervalAction(const QJsonObject &action)
{
    QJsonArray array;
    array.append(action);
    post("api/v3/intervalaction", array, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Interval action added" : error);
    });
}

void SupportClient::deleteIntervalAction(const QString &name)
{
    deleteRequest("api/v3/intervalaction/name/" + name, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Interval action deleted" : error);
    });
}

void SupportClient::get(const QString &path)
{
    QString fullPath = baseUrl();
    if (!fullPath.endsWith('/') && !path.startsWith('/')) fullPath += "/";
    
    QNetworkRequest request(QUrl(fullPath + path));
    QNetworkReply* reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, path]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if (path.contains("notification")) emit notificationsReceived(doc.object()["notifications"].toArray());
            else if (path.contains("subscription")) emit subscriptionsReceived(doc.object()["subscriptions"].toArray());
            else if (path.contains("intervalaction")) emit intervalActionsReceived(doc.object()["actions"].toArray());
            else if (path.contains("interval")) emit intervalsReceived(doc.object()["intervals"].toArray());
        }
        reply->deleteLater();
    });
}
