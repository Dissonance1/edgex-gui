#include "DeviceServicesModel.h"
#include <QDateTime>

DeviceServicesModel::DeviceServicesModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers << "Name" << "Description" << "Admin State" << "Operating State" << "Last Connected";
}

void DeviceServicesModel::setServices(const QJsonArray &services)
{
    beginResetModel();
    m_services = services;
    endResetModel();
}

int DeviceServicesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_services.size();
}

int DeviceServicesModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_headers.size();
}

QVariant DeviceServicesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_services.size())
        return QVariant();

    if (role == Qt::DisplayRole) {
        QJsonObject service = m_services[index.row()].toObject();
        switch (index.column()) {
            case 0: return service["name"].toString();
            case 1: return service["description"].toString();
            case 2: return service["adminState"].toString();
            case 3: return service["operatingState"].toString();
            case 4: {
                qlonglong ts = service["lastConnected"].toVariant().toLongLong();
                return ts > 0 ? QDateTime::fromMSecsSinceEpoch(ts).toString() : "N/A";
            }
        }
    }
    return QVariant();
}

QVariant DeviceServicesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section < m_headers.size())
            return m_headers[section];
    }
    return QVariant();
}
