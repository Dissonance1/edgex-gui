#ifndef SUPPORTCLIENT_H
#define SUPPORTCLIENT_H

#include "EdgexClientBase.h"
#include <QJsonArray>
#include <QJsonObject>

class SupportClient : public EdgexClientBase
{
    Q_OBJECT
public:
    explicit SupportClient(QObject *parent = nullptr);

    // Notifications
    void fetchNotifications();
    void deleteNotification(const QString &id);
    void cleanupNotifications();

    void fetchSubscriptions();
    void addSubscription(const QJsonObject &subscription);
    void deleteSubscription(const QString &name);

    // Scheduler
    void fetchIntervals();
    void fetchIntervalActions();
    void addInterval(const QJsonObject &interval);
    void deleteInterval(const QString &name);
    void addIntervalAction(const QJsonObject &action);
    void deleteIntervalAction(const QString &name);

private:
    void get(const QString &path);

signals:
    void notificationsReceived(const QJsonArray &notifications);
    void subscriptionsReceived(const QJsonArray &subscriptions);
    void intervalsReceived(const QJsonArray &intervals);
    void intervalActionsReceived(const QJsonArray &actions);
    void operationCompleted(bool success, const QString &message);
};

#endif // SUPPORTCLIENT_H
