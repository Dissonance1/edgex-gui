#ifndef APPSERVICECLIENT_H
#define APPSERVICECLIENT_H

#include "EdgexClientBase.h"
#include <QJsonArray>
#include <QJsonObject>

class AppServiceClient : public EdgexClientBase
{
    Q_OBJECT
public:
    explicit AppServiceClient(QObject *parent = nullptr);

    void fetchAppServices();
    void fetchServiceConfig(const QString &serviceUrl);
    void updateServiceConfig(const QString &serviceUrl, const QJsonObject &config);

signals:
    void appServicesReceived(const QJsonArray &services);
    void configReceived(const QJsonObject &config);
    void operationCompleted(bool success, const QString &message);
    void errorOccurred(const QString &message);
};

#endif // APPSERVICECLIENT_H
