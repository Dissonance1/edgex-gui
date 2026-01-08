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

private slots:
    void refresh();
    void onNotificationsReceived(const QJsonArray &notifications);
    void onSubscriptionsReceived(const QJsonArray &subscriptions);
    void onOperationCompleted(bool success, const QString &message);
    
    void onAddSubscription();
    void onDeleteSubscription();
    void onDeleteNotification();
    void onCleanup();

private:
    Ui::NotificationView *ui;
    SupportClient *m_client;
};

#endif // NOTIFICATIONVIEW_H
