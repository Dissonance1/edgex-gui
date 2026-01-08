#ifndef EVENTSMODEL_H
#define EVENTSMODEL_H

#include <QAbstractTableModel>
#include <QJsonArray>
#include <QJsonObject>

class EventsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit EventsModel(QObject *parent = nullptr);

    void setEvents(const QJsonArray &events);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    QJsonArray m_events;
    QStringList m_headers;
};

#endif // EVENTSMODEL_H
