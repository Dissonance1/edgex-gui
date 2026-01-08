#ifndef PROFILESVIEW_H
#define PROFILESVIEW_H

#include <QWidget>
#include <QTableView>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include "ProfilesModel.h"
#include "MetadataClient.h"

namespace Ui {
class ProfilesView;
}

class ProfilesView : public QWidget
{
    Q_OBJECT
public:
    explicit ProfilesView(QWidget *parent = nullptr);
    ~ProfilesView();

private slots:
    void refresh();
    void onProfilesReceived(const QJsonArray &profiles);
    void onSearch(const QString &text);
    void onAddProfile();
    void onOperationCompleted(bool success, const QString &message);

private:
    Ui::ProfilesView *ui;
    ProfilesModel *m_model;
    QSortFilterProxyModel *m_proxyModel;
    MetadataClient *m_client;
};

#endif // PROFILESVIEW_H
