#ifndef DEVICESERVICESMODEL_H
#define DEVICESERVICESMODEL_H

#include <QAbstractTableModel>
#include <QJsonArray>
#include <QJsonObject>

class DeviceServicesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit DeviceServicesModel(QObject *parent = nullptr);

    void setServices(const QJsonArray &services);
    QJsonObject getService(int row) const { return m_services[row].toObject(); }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    QJsonArray m_services;
    QStringList m_headers;
};

#endif // DEVICESERVICESMODEL_H
