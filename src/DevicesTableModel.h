#ifndef DEVICESTABLEMODEL_H
#define DEVICESTABLEMODEL_H

#include <QAbstractTableModel>
#include <QJsonArray>
#include <QJsonObject>

class DevicesTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DevicesTableModel(QObject *parent = nullptr);

    void setDevices(const QJsonArray &devices);
    QJsonObject getDevice(int row) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    QJsonArray m_devices;
    QStringList m_headers;
};

#endif // DEVICESTABLEMODEL_H
