#include "EventsModel.h"
#include <QDateTime>

EventsModel::EventsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers << "ID" << "Device" << "Readings" << "Created" << "Origin";
}

void EventsModel::setEvents(const QJsonArray &events)
{
    beginResetModel();
    m_events = events;
    endResetModel();
}

int EventsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_events.size();
}

int EventsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_headers.size();
}

QVariant EventsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_events.size())
        return QVariant();

    if (role == Qt::DisplayRole) {
        QJsonObject event = m_events[index.row()].toObject();
        switch (index.column()) {
            case 0: return event["id"].toString();
            case 1: return event["deviceName"].toString();
            case 2: return event["readings"].toArray().size();
            case 3: {
                qlonglong ts = event["origin"].toVariant().toLongLong();
                if (ts == 0) ts = event["created"].toVariant().toLongLong();
                if (ts > 1000000000000000LL) ts /= 1000000; // Nanoseconds to Milliseconds
                return QDateTime::fromMSecsSinceEpoch(ts).toLocalTime().toString("yyyy-MM-dd HH:mm:ss");
            }
            case 4: {
                return event["origin"].toVariant().toString();
            }
        }
    }
    return QVariant();
}

QVariant EventsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section < m_headers.size())
            return m_headers[section];
    }
    return QVariant();
}
