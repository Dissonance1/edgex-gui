#ifndef SYSTEMCLIENT_H
#define SYSTEMCLIENT_H

#include "EdgexClientBase.h"
#include <QList>
#include <QPair>

class SystemClient : public EdgexClientBase
{
    Q_OBJECT
public:
    explicit SystemClient(QObject *parent = nullptr);

    void checkServiceStatus(const QString &serviceName, const QString &serviceUrl);
    void fetchRegisteredServices(const QString &registryUrl);

signals:
    void statusReceived(const QString &serviceName, bool isUp);
    void servicesFetched(const QList<QPair<QString, QString>> &services); // name, url
};

#endif // SYSTEMCLIENT_H
