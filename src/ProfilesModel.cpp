#include "ProfilesModel.h"

ProfilesModel::ProfilesModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_headers << "ID" << "Name" << "Description" << "Manufacturer" << "Model" << "Labels";
}

void ProfilesModel::setProfiles(const QJsonArray &profiles)
{
    beginResetModel();
    m_profiles = profiles;
    endResetModel();
}

int ProfilesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_profiles.size();
}

int ProfilesModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_headers.size();
}

QVariant ProfilesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_profiles.size())
        return QVariant();

    if (role == Qt::DisplayRole) {
        QJsonObject profile = m_profiles[index.row()].toObject();
        switch (index.column()) {
            case 0: return profile["id"].toString();
            case 1: return profile["name"].toString();
            case 2: return profile["description"].toString();
            case 3: return profile["manufacturer"].toString();
            case 4: return profile["model"].toString();
            case 5: {
                QStringList labels;
                for (const QJsonValue &v : profile["labels"].toArray()) {
                    labels << v.toString();
                }
                return labels.join(", ");
            }
        }
    }
    return QVariant();
}

QVariant ProfilesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if (section < m_headers.size())
            return m_headers[section];
    }
    return QVariant();
}
