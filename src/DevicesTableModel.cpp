#include "DevicesTableModel.h"
#include <QDateTime>

DevicesTableModel::DevicesTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers << "ID" << "Name" << "Description" << "Labels" << "Admin State" << "Operating State" << "Commands" << "AutoEvents" << "Profile" << "Service";
}

void DevicesTableModel::setDevices(const QJsonArray &devices)
{
    beginResetModel();
    m_devices = devices;
    endResetModel();
}

QJsonObject DevicesTableModel::getDevice(int row) const
{
    if (row < 0 || row >= m_devices.size()) return QJsonObject();
    return m_devices[row].toObject();
}

int DevicesTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_devices.size();
}

int DevicesTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_headers.size();
}

QVariant DevicesTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_devices.size())
        return QVariant();

    if (role == Qt::DisplayRole) {
        QJsonObject device = m_devices[index.row()].toObject();
        switch (index.column()) {
            case 0: return device["id"].toString();
            case 1: return device["name"].toString();
            case 2: return device["description"].toString();
            case 3: {
                QStringList labels;
                for (const QJsonValue &v : device["labels"].toArray()) labels << v.toString();
                return labels.join(", ");
            }
            case 4: return device["adminState"].toString();
            case 5: return device["operatingState"].toString();
            case 6: return "Terminal";
            case 7: return "Calendar";
            case 8: return device["profileName"].toString();
            case 9: return device["serviceName"].toString();
        }
    }
    return QVariant();
}

QVariant DevicesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section < m_headers.size())
            return m_headers[section];
    }
    return QVariant();
}
