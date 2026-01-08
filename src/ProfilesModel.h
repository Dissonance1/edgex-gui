#ifndef PROFILESMODEL_H
#define PROFILESMODEL_H

#include <QAbstractTableModel>
#include <QJsonArray>
#include <QJsonObject>

class ProfilesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ProfilesModel(QObject *parent = nullptr);

    void setProfiles(const QJsonArray &profiles);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    QJsonArray m_profiles;
    QStringList m_headers;
};

#endif // PROFILESMODEL_H
