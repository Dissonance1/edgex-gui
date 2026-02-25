#ifndef NOTIFICATIONPOPUP_H
#define NOTIFICATIONPOPUP_H

#include <QWidget>
#include <QJsonArray>
#include <QTableWidgetItem>
#include "SupportClient.h"

namespace Ui {
class NotificationPopup;
}

class NotificationPopup : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationPopup(QWidget *parent = nullptr);
    ~NotificationPopup();

    void fetchNotifications();

signals:
    void viewAllRequested();

protected:
    void focusOutEvent(QFocusEvent *event) override;
    bool event(QEvent *event) override;

private slots:
    void onNotificationsReceived(const QJsonArray &notifications);
    void onTabChanged(int index);

private:
    void updateList(const QJsonArray &notifications);
    QString formatTimestamp(long long ts);

    Ui::NotificationPopup *ui;
    SupportClient *m_client;
    QString m_currentStatus;
};

#endif // NOTIFICATIONPOPUP_H
