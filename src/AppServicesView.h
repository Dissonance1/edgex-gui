#ifndef APPSERVICESVIEW_H
#define APPSERVICESVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QJsonArray>
#include "AppServiceClient.h"

namespace Ui {
class AppServicesView;
}

class AppServicesView : public QWidget
{
    Q_OBJECT
public:
    explicit AppServicesView(QWidget *parent = nullptr);
    ~AppServicesView();

private slots:
    void refresh();
    void onAppServicesReceived(const QJsonArray &services);
    void onViewConfig();
    void onConfigReceived(const QJsonObject &config);
    void onOperationCompleted(bool success, const QString &message);
    void onSearch(const QString &text);
    void onAddFromTemplate();

private:
    Ui::AppServicesView *ui;
    AppServiceClient *m_client;
    QJsonArray m_currentServices;
};

#endif // APPSERVICESVIEW_H
