#ifndef READINGSMODEL_H
#define READINGSMODEL_H

#include <QAbstractTableModel>
#include <QJsonArray>
#include <QJsonObject>

class ReadingsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ReadingsModel(QObject *parent = nullptr);

    void setReadings(const QJsonArray &readings);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    QJsonArray m_readings;
    QStringList m_headers;
};

#endif // READINGSMODEL_H
