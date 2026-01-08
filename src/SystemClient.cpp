#include "SystemClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

SystemClient::SystemClient(QObject *parent)
    : EdgexClientBase(parent)
{
}

void SystemClient::checkServiceStatus(const QString &serviceName, const QString &serviceUrl)
{
    // Use the provided serviceUrl for health check
    QUrl url(serviceUrl + "/api/v3/ping");
    QNetworkRequest request(url);
    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, serviceName]() {
        bool isUp = (reply->error() == QNetworkReply::NoError);
        emit statusReceived(serviceName, isUp);
        reply->deleteLater();
    });
}

void SystemClient::fetchRegisteredServices(const QString &registryUrl)
{
    QUrl url(registryUrl + "/v1/agent/services");
    QNetworkRequest request(url);
    QNetworkReply *reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QList<QPair<QString, QString>> services;
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject root = doc.object();
            
            // Consul /v1/agent/services returns a map of ID -> Service Info
            for (auto it = root.begin(); it != root.end(); ++it) {
                QJsonObject svc = it.value().toObject();
                QString name = svc["Service"].toString();
                QString address = svc["Address"].toString();
                int port = svc["Port"].toInt();
                
                // Construct URL (assume HTTP)
                QString svcUrl = QString("http://%1:%2").arg(address).arg(port);
                services.append({name, svcUrl});
            }
            emit servicesFetched(services);
        } else {
            qDebug() << "Failed to fetch services from Consul:" << reply->errorString();
        }
        reply->deleteLater();
    });
}
