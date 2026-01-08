#ifndef SYSTEMSTATUSVIEW_H
#define SYSTEMSTATUSVIEW_H

#include <QWidget>
#include <QPair>
#include <QList>
#include <QTableView>
#include <QTimer>
#include "SystemClient.h"
#include "ServiceStatusModel.h"

namespace Ui {
class SystemStatusView;
}

class SystemStatusView : public QWidget
{
    Q_OBJECT
public:
    explicit SystemStatusView(QWidget *parent = nullptr);
    ~SystemStatusView();

private slots:
    void refresh();
    void onStatusReceived(const QString &name, bool isUp);
    void onServicesFetched(const QList<QPair<QString, QString>> &services); // NEW

private:
    Ui::SystemStatusView *ui;
    ServiceStatusModel *m_model;
    SystemClient *m_client;
    QTimer *m_timer;
};

#endif // SYSTEMSTATUSVIEW_H
