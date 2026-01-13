#ifndef COREDATACLIENT_H
#define COREDATACLIENT_H

#include "EdgexClientBase.h"

class CoreDataClient : public EdgexClientBase
{
    Q_OBJECT
public:
    explicit CoreDataClient(QObject *parent = nullptr);

    void fetchEvents(const QString &deviceName = QString(), int limit = 200);
    void fetchReadings(const QString &deviceName = QString(), int limit = 200);
    void deleteEvent(const QString &id);
    void deleteAllEvents();
    void deleteAllReadings();

signals:
    void eventsReceived(const QJsonArray &events);
    void readingsReceived(const QJsonArray &readings);
    void operationCompleted(bool success, const QString &message);
};

#endif // COREDATACLIENT_H
