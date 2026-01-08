#ifndef DASHBOARDVIEW_H
#define DASHBOARDVIEW_H

#include <QWidget>
#include <QLabel>
#include "MetadataClient.h"
#include "CoreDataClient.h"
#include "SupportClient.h"

namespace Ui {
class DashboardView;
}

class DashboardView : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardView(QWidget *parent = nullptr);
    ~DashboardView();

private slots:
    void refresh();
    void onDevicesReceived(const QJsonArray &devices);
    void onServicesReceived(const QJsonArray &services);
    void onProfilesReceived(const QJsonArray &profiles);
    void onEventsReceived(const QJsonArray &events);
    void onReadingsReceived(const QJsonArray &readings);
    void onNotificationsReceived(const QJsonArray &notifications);
    void onIntervalsReceived(const QJsonArray &intervals);

private:
    Ui::DashboardView *ui;
    MetadataClient *m_metadataClient;
    CoreDataClient *m_coreDataClient;
    SupportClient *m_supportClient;

    // Card Labels (pointers into created widgets)
    QLabel *m_serviceCount = nullptr;
    QLabel *m_serviceLockedCount = nullptr;
    QLabel *m_deviceCount = nullptr;
    QLabel *m_deviceLockedCount = nullptr;
    QLabel *m_profileCount = nullptr;
    QLabel *m_profileSubLabel = nullptr;
    QLabel *m_schedulerCount = nullptr;
    QLabel *m_schedulerSubLabel = nullptr;
    QLabel *m_notificationCount = nullptr;
    QLabel *m_notificationSubLabel = nullptr;
    QLabel *m_eventCount = nullptr;
    QLabel *m_eventSubLabel = nullptr;
    QLabel *m_readingCount = nullptr;
    QLabel *m_readingSubLabel = nullptr;
};

#endif // DASHBOARDVIEW_H
