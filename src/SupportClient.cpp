#include "SupportClient.h"
#include <QJsonDocument>
#include <QJsonObject>

SupportClient::SupportClient(QObject *parent)
    : EdgexClientBase(parent)
{
}

void SupportClient::fetchNotifications(int offset, int limit)
{
    // EdgeX V3 doesn't have an "/all" route for notifications.
    // We use a large time range as a substitute to fetch "all".
    // Using 0 to a very large number (yr 2100 ~ 4102444800000)
    fetchNotificationsByTimeRange(0, 4102444800000LL, offset, limit);
}

void SupportClient::fetchNotificationsByStatus(const QString &status, int offset, int limit)
{
    get(QString("api/v3/notification/status/%1?offset=%2&limit=%3").arg(status).arg(offset).arg(limit));
}

void SupportClient::fetchNotificationsByTimeRange(long long start, long long end, int offset, int limit)
{
    get(QString("api/v3/notification/start/%1/end/%2?offset=%3&limit=%4").arg(start).arg(end).arg(offset).arg(limit));
}

void SupportClient::deleteNotification(const QString &id)
{
    deleteRequest("api/v3/notification/id/" + id, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Notification deleted" : error);
    });
}

void SupportClient::cleanupNotifications()
{
    deleteRequest("api/v3/notification/cleanup", [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Notifications cleaned up" : error);
    });
}

void SupportClient::cleanupNotificationsByAge(long long age)
{
    deleteRequest("api/v3/notification/age/" + QString::number(age), [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Old notifications cleaned up" : error);
    });
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

void SupportClient::updateSubscription(const QJsonObject &subscription)
{
    QJsonArray array;
    array.append(subscription);
    patch("api/v3/subscription", array, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Subscription updated" : error);
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
    if (fullPath.isEmpty()) {
        qDebug() << "SupportClient: baseUrl is empty! Cannot fetch" << path;
        return;
    }
    if (!fullPath.endsWith('/') && !path.startsWith('/')) fullPath += "/";
    
    qDebug() << "SupportClient fetching:" << (fullPath + path);
    QNetworkRequest request(QUrl(fullPath + path));
    QNetworkReply* reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, path]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonArray arr;
            if (path.contains("notification")) {
                arr = doc.object()["notifications"].toArray();
                qDebug() << "SupportClient: Received" << arr.size() << "notifications";
                emit notificationsReceived(arr);
            } else if (path.contains("subscription")) {
                arr = doc.object()["subscriptions"].toArray();
                emit subscriptionsReceived(arr);
            } else if (path.contains("intervalaction")) {
                arr = doc.object()["actions"].toArray();
                emit intervalActionsReceived(arr);
            } else if (path.contains("interval")) {
                arr = doc.object()["intervals"].toArray();
                emit intervalsReceived(arr);
            }
        } else {
            qDebug() << "SupportClient: Error fetching" << path << ":" << reply->errorString();
        }
        reply->deleteLater();
    });
}
