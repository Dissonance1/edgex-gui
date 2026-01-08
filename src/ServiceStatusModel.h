#ifndef SERVICESTATUSMODEL_H
#define SERVICESTATUSMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QDateTime>

struct ServiceStatus {
    QString name;
    QString url;
    bool isUp;
    QDateTime lastCheck;
};

class ServiceStatusModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ServiceStatusModel(QObject *parent = nullptr);

    void updateStatus(const QString &name, bool isUp);
    void addService(const QString &name, const QString &url);
    void clear();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    QList<ServiceStatus> m_services;
    QStringList m_headers;
};

#endif // SERVICESTATUSMODEL_H
