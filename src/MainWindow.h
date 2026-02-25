#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QStackedWidget>
#include <QTimer>
#include "SupportClient.h"

class DataIngestionServer;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void notificationsUpdated();

private slots:
    void onNavItemChanged(int index);
    void showSettings();
    void testConnection();
    void onPollingNotificationsReceived(const QJsonArray &notifications);

private:
    void setupNavigation();
    void createMenus();

    Ui::MainWindow *ui;
    // Pointers to widgets now managed by UI, but we might keep convenience pointers or use ui-> directly
    // m_navPanel is now ui->navPanel
    // m_centralStack is now ui->centralStack
    
    DataIngestionServer *m_ingestionServer;
    SupportClient *m_pollClient;
    QTimer *m_pollTimer;
    QString m_lastNotificationId;
};

#endif // MAINWINDOW_H
