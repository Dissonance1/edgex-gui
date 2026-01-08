#include "ServiceStatusModel.h"
#include <QColor>

ServiceStatusModel::ServiceStatusModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers << "Service" << "URL" << "Status" << "Last Check";
}

void ServiceStatusModel::addService(const QString &name, const QString &url)
{
    beginInsertRows(QModelIndex(), m_services.size(), m_services.size());
    m_services.append({name, url, false, QDateTime()});
    endInsertRows();
}

void ServiceStatusModel::clear()
{
    beginResetModel();
    m_services.clear();
    endResetModel();
}

void ServiceStatusModel::updateStatus(const QString &name, bool isUp)
{
    for (int i = 0; i < m_services.size(); ++i) {
        if (m_services[i].name == name) {
            m_services[i].isUp = isUp;
            m_services[i].lastCheck = QDateTime::currentDateTime();
            emit dataChanged(index(i, 2), index(i, 3));
            return;
        }
    }
}

int ServiceStatusModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_services.size();
}

int ServiceStatusModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_headers.size();
}

QVariant ServiceStatusModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_services.size())
        return QVariant();

    const auto &service = m_services[index.row()];

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return service.name;
            case 1: return service.url;
            case 2: return service.isUp ? "UP" : "DOWN";
            case 3: return service.lastCheck.isValid() ? service.lastCheck.toString("hh:mm:ss") : "Never";
        }
    } else if (role == Qt::ForegroundRole && index.column() == 2) {
        return service.isUp ? QVariant(QColor(Qt::green)) : QVariant(QColor(Qt::red));
    }
    return QVariant();
}

QVariant ServiceStatusModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section < m_headers.size())
            return m_headers[section];
    }
    return QVariant();
}
