#include "CoreDataClient.h"

CoreDataClient::CoreDataClient(QObject *parent)
    : EdgexClientBase(parent)
{
}

void CoreDataClient::fetchEvents(const QString &deviceName, int limit)
{
    QString path = "api/v3/event/all?limit=" + QString::number(limit);
    if (!deviceName.isEmpty()) {
        path = "api/v3/event/device/name/" + deviceName + "?limit=" + QString::number(limit);
    }

    QNetworkReply *reply = get(path);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit eventsReceived(doc.object()["events"].toArray());
        } else {
            emit errorOccurred(reply->errorString());
        }
        reply->deleteLater();
    });
}

void CoreDataClient::fetchReadings(const QString &deviceName, int limit)
{
    QString path = "api/v3/reading/all?limit=" + QString::number(limit);
    if (!deviceName.isEmpty()) {
        path = "api/v3/reading/device/name/" + deviceName + "?limit=" + QString::number(limit);
    }

    QNetworkReply *reply = get(path);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit readingsReceived(doc.object()["readings"].toArray());
        } else {
            emit errorOccurred(reply->errorString());
        }
        reply->deleteLater();
    });
}

void CoreDataClient::deleteEvent(const QString &id)
{
    deleteRequest("api/v3/event/id/" + id, [this](const QByteArray&, bool success, const QString& error) {
        emit operationCompleted(success, success ? "Event deleted" : error);
    });
}

void CoreDataClient::deleteAllEvents()
{
    deleteRequest("api/v3/event/all", [this](const QByteArray&, bool success, const QString& error) {
        emit operationCompleted(success, success ? "All events deleted" : error);
    });
}

void CoreDataClient::deleteAllReadings()
{
    deleteRequest("api/v3/reading/all", [this](const QByteArray&, bool success, const QString& error) {
        emit operationCompleted(success, success ? "All readings deleted" : error);
    });
}
