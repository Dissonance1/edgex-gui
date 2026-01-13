/********************************************************************************
** Form generated from reading UI file 'SystemStatusView.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SYSTEMSTATUSVIEW_H
#define UI_SYSTEMSTATUSVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SystemStatusView
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *toolbar;
    QPushButton *btnRefresh;
    QSpacerItem *horizontalSpacer;
    QTableView *tableView;

    void setupUi(QWidget *SystemStatusView)
    {
        if (SystemStatusView->objectName().isEmpty())
            SystemStatusView->setObjectName("SystemStatusView");
        SystemStatusView->resize(800, 600);
        verticalLayout = new QVBoxLayout(SystemStatusView);
        verticalLayout->setObjectName("verticalLayout");
        toolbar = new QHBoxLayout();
        toolbar->setObjectName("toolbar");
        btnRefresh = new QPushButton(SystemStatusView);
        btnRefresh->setObjectName("btnRefresh");

        toolbar->addWidget(btnRefresh);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        toolbar->addItem(horizontalSpacer);


        verticalLayout->addLayout(toolbar);

        tableView = new QTableView(SystemStatusView);
        tableView->setObjectName("tableView");
        tableView->horizontalHeader()->setStretchLastSection(true);

        verticalLayout->addWidget(tableView);


        retranslateUi(SystemStatusView);

        QMetaObject::connectSlotsByName(SystemStatusView);
    } // setupUi

    void retranslateUi(QWidget *SystemStatusView)
    {
        btnRefresh->setText(QCoreApplication::translate("SystemStatusView", "Refresh Status", nullptr));
        (void)SystemStatusView;
    } // retranslateUi

};

namespace Ui {
    class SystemStatusView: public Ui_SystemStatusView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SYSTEMSTATUSVIEW_H
