#include "AppServiceClient.h"
#include <QJsonDocument>

AppServiceClient::AppServiceClient(QObject *parent)
    : EdgexClientBase(parent)
{
}

void AppServiceClient::fetchAppServices()
{
    // In V3, registry (core-keeper) manages all services
    QNetworkReply *reply = get("api/v3/registry/service/all");
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonArray all = doc.object()["services"].toArray();
            QJsonArray apps;
            for (const auto &v : all) {
                QString name = v.toObject()["serviceId"].toString();
                if (name.contains("app-") || name.contains("app-service")) {
                    apps.append(v);
                }
            }
            emit appServicesReceived(apps);
        } else {
            emit errorOccurred(reply->errorString());
        }
        reply->deleteLater();
    });
}

void AppServiceClient::fetchServiceConfig(const QString &serviceKey)
{
    // System service port 59890 /api/v3/registry/config/{serviceKey}
    QNetworkReply *reply = get("api/v3/registry/config/" + serviceKey);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit configReceived(doc.object());
        } else {
            emit errorOccurred(reply->errorString());
        }
        reply->deleteLater();
    });
}

void AppServiceClient::updateServiceConfig(const QString &serviceUrl, const QJsonObject &config)
{
    if (serviceUrl.startsWith("http")) {
        setBaseUrl(serviceUrl);
        put("api/v3/config", config, [this](const QByteArray&, bool success, const QString &error) {
            emit operationCompleted(success, success ? "Configuration updated" : error);
        });
    } else {
        put("api/v3/registry/config/" + serviceUrl, config, [this](const QByteArray&, bool success, const QString &error) {
            emit operationCompleted(success, success ? "Configuration updated via registry" : error);
        });
    }
}
