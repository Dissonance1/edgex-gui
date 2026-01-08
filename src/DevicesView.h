#ifndef DEVICESVIEW_H
#define DEVICESVIEW_H

#include <QWidget>
#include <QTableView>
#include <QLineEdit>
#include "MetadataClient.h"
#include "DevicesTableModel.h"

namespace Ui {
class DevicesView;
}

class DevicesView : public QWidget
{
    Q_OBJECT

public:
    explicit DevicesView(QWidget *parent = nullptr);
    ~DevicesView();

private slots:
    void refresh();
    void onDevicesReceived(const QJsonArray &devices);
    void onOperationCompleted(bool success, const QString &message);
    
    // Actions
    void onAddDevice();
    // void onViewDevice();
    void onSearch(const QString &text);

private:
    void populateTable(); // Only if manual tweaks needed, but model handles data

    Ui::DevicesView *ui;
    MetadataClient *m_client;
    // widgets are in ui->...
    DevicesTableModel *m_model;
    QJsonArray m_currentDevices;
};

#endif // DEVICESVIEW_H
