#ifndef NOTIFICATIONVIEW_H
#define NOTIFICATIONVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QJsonArray>
#include "SupportClient.h"

namespace Ui {
class NotificationView;
}

class NotificationView : public QWidget
{
    Q_OBJECT
public:
    explicit NotificationView(QWidget *parent = nullptr);
    ~NotificationView();

public slots:
    void refresh();

private slots:
    void onNotificationsReceived(const QJsonArray &notifications);
    void onSubscriptionsReceived(const QJsonArray &subscriptions);
    void onOperationCompleted(bool success, const QString &message);
    
    void onAddSubscription();
    void onEditSubscription();
    void onDeleteSubscription();
    void onDeleteNotification();
    void onCleanup();
    void onCleanupByAge();
    void onSearch();
    void onStatusChanged(int index);
    void onToggleAdvanced(bool checked);
    void onNotificationSelected(QTableWidgetItem *item);

private:
    Ui::NotificationView *ui;
    SupportClient *m_client;
    QJsonArray m_lastNotifications;
    QJsonArray m_lastSubscriptions;
};

#endif // NOTIFICATIONVIEW_H
