#ifndef DEVICESERVICESVIEW_H
#define DEVICESERVICESVIEW_H

#include <QWidget>
#include <QTableView>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include "DeviceServicesModel.h"
#include "MetadataClient.h"

namespace Ui {
class DeviceServicesView;
}

class DeviceServicesView : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceServicesView(QWidget *parent = nullptr);
    ~DeviceServicesView();

private slots:
    void refresh();
    void onServicesReceived(const QJsonArray &services);
    void onSearch(const QString &text);
    void onEditService();
    void onDeleteService();
    void onLockService();
    void onUnlockService();
    void onOperationCompleted(bool success, const QString &message);

private:
    Ui::DeviceServicesView *ui;
    DeviceServicesModel *m_model;
    QSortFilterProxyModel *m_proxyModel;
    MetadataClient *m_client;
};

#endif // DEVICESERVICESVIEW_H
