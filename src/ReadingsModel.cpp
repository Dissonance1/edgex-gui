#include "ReadingsModel.h"
#include <QDateTime>

ReadingsModel::ReadingsModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers << "Resource" << "Value" << "Type" << "Unit" << "Origin";
}

void ReadingsModel::setReadings(const QJsonArray &readings)
{
    beginResetModel();
    m_readings = readings;
    endResetModel();
}

int ReadingsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_readings.size();
}

int ReadingsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_headers.size();
}

QVariant ReadingsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_readings.size())
        return QVariant();

    if (role == Qt::DisplayRole) {
        QJsonObject reading = m_readings[index.row()].toObject();
        switch (index.column()) {
            case 0: return reading["resourceName"].toString();
            case 1: return reading["value"].toString(); // Can be complex but simplified here
            case 2: return reading["valueType"].toString();
            case 3: return reading["units"].toString();
            case 4: {
                qlonglong ts = reading["origin"].toVariant().toLongLong();
                return QDateTime::fromMSecsSinceEpoch(ts/1000000).toString(); // EdgeX origin is usually nanoseconds
            }
        }
    }
    return QVariant();
}

QVariant ReadingsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section < m_headers.size())
            return m_headers[section];
    }
    return QVariant();
}
