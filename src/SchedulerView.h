#ifndef SCHEDULERVIEW_H
#define SCHEDULERVIEW_H

#include <QWidget>
#include <QTableWidget>
#include <QJsonArray>
#include "SupportClient.h"

namespace Ui {
class SchedulerView;
}

class SchedulerView : public QWidget
{
    Q_OBJECT
public:
    explicit SchedulerView(QWidget *parent = nullptr);
    ~SchedulerView();

private slots:
    void refresh();
    void onIntervalsReceived(const QJsonArray &intervals);
    void onActionsReceived(const QJsonArray &actions);
    void onOperationCompleted(bool success, const QString &message);
    
    void onAddInterval();
    void onDeleteInterval();
    void onAddAction();
    void onDeleteAction();

private:
    Ui::SchedulerView *ui;
    SupportClient *m_client;
};

#endif // SCHEDULERVIEW_H
