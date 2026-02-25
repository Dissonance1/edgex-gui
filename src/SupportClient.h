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
    void fetchNotifications(int offset = 0, int limit = 20);
    void fetchNotificationsByStatus(const QString &status, int offset = 0, int limit = 20);
    void fetchNotificationsByTimeRange(long long start, long long end, int offset = 0, int limit = 20);
    void deleteNotification(const QString &id);
    void cleanupNotifications();
    void cleanupNotificationsByAge(long long age);

    void fetchSubscriptions();
    void addSubscription(const QJsonObject &subscription);
    void updateSubscription(const QJsonObject &subscription);
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
