#ifndef EVENTSREADINGSVIEW_H
#define EVENTSREADINGSVIEW_H

#include <QWidget>
#include <QTableView>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QJsonObject>
#include <QJsonArray>
#include "CoreDataClient.h"
#include "EventsModel.h"
#include "ReadingsModel.h"

namespace Ui {
class EventsReadingsView;
}

class EventsReadingsView : public QWidget
{
    Q_OBJECT
public:
    explicit EventsReadingsView(QWidget *parent = nullptr);
    ~EventsReadingsView();

private slots:
    void refresh();
    void onEventsReceived(const QJsonArray &events);
    void onEventSelected(const QModelIndex &index);
    void onSearchEvents(const QString &text);
    void onOperationCompleted(bool success, const QString &message);

private:
    Ui::EventsReadingsView *ui;
    EventsModel *m_eventsModel;
    ReadingsModel *m_readingsModel;
    QSortFilterProxyModel *m_eventsProxy;
    CoreDataClient *m_client;
    QJsonArray m_currentEvents;
};

#endif // EVENTSREADINGSVIEW_H
