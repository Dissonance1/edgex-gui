/********************************************************************************
** Form generated from reading UI file 'SchedulerView.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCHEDULERVIEW_H
#define UI_SCHEDULERVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SchedulerView
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *intervalsTab;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *intToolbar;
    QPushButton *btnRefInt;
    QPushButton *btnAddInt;
    QPushButton *btnDelInt;
    QSpacerItem *horizontalSpacer;
    QTableWidget *intervalsTable;
    QWidget *actionsTab;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *actToolbar;
    QPushButton *btnRefAct;
    QPushButton *btnAddAct;
    QPushButton *btnDelAct;
    QSpacerItem *horizontalSpacer_2;
    QTableWidget *actionsTable;

    void setupUi(QWidget *SchedulerView)
    {
        if (SchedulerView->objectName().isEmpty())
            SchedulerView->setObjectName("SchedulerView");
        SchedulerView->resize(800, 600);
        verticalLayout = new QVBoxLayout(SchedulerView);
        verticalLayout->setObjectName("verticalLayout");
        tabWidget = new QTabWidget(SchedulerView);
        tabWidget->setObjectName("tabWidget");
        intervalsTab = new QWidget();
        intervalsTab->setObjectName("intervalsTab");
        verticalLayout_2 = new QVBoxLayout(intervalsTab);
        verticalLayout_2->setObjectName("verticalLayout_2");
        intToolbar = new QHBoxLayout();
        intToolbar->setObjectName("intToolbar");
        btnRefInt = new QPushButton(intervalsTab);
        btnRefInt->setObjectName("btnRefInt");

        intToolbar->addWidget(btnRefInt);

        btnAddInt = new QPushButton(intervalsTab);
        btnAddInt->setObjectName("btnAddInt");

        intToolbar->addWidget(btnAddInt);

        btnDelInt = new QPushButton(intervalsTab);
        btnDelInt->setObjectName("btnDelInt");

        intToolbar->addWidget(btnDelInt);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        intToolbar->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(intToolbar);

        intervalsTable = new QTableWidget(intervalsTab);
        if (intervalsTable->columnCount() < 2)
            intervalsTable->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        intervalsTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        intervalsTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        intervalsTable->setObjectName("intervalsTable");
        intervalsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout_2->addWidget(intervalsTable);

        tabWidget->addTab(intervalsTab, QString());
        actionsTab = new QWidget();
        actionsTab->setObjectName("actionsTab");
        verticalLayout_3 = new QVBoxLayout(actionsTab);
        verticalLayout_3->setObjectName("verticalLayout_3");
        actToolbar = new QHBoxLayout();
        actToolbar->setObjectName("actToolbar");
        btnRefAct = new QPushButton(actionsTab);
        btnRefAct->setObjectName("btnRefAct");

        actToolbar->addWidget(btnRefAct);

        btnAddAct = new QPushButton(actionsTab);
        btnAddAct->setObjectName("btnAddAct");

        actToolbar->addWidget(btnAddAct);

        btnDelAct = new QPushButton(actionsTab);
        btnDelAct->setObjectName("btnDelAct");

        actToolbar->addWidget(btnDelAct);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        actToolbar->addItem(horizontalSpacer_2);


        verticalLayout_3->addLayout(actToolbar);

        actionsTable = new QTableWidget(actionsTab);
        if (actionsTable->columnCount() < 3)
            actionsTable->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        actionsTable->setHorizontalHeaderItem(0, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        actionsTable->setHorizontalHeaderItem(1, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        actionsTable->setHorizontalHeaderItem(2, __qtablewidgetitem4);
        actionsTable->setObjectName("actionsTable");
        actionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout_3->addWidget(actionsTable);

        tabWidget->addTab(actionsTab, QString());

        verticalLayout->addWidget(tabWidget);


        retranslateUi(SchedulerView);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SchedulerView);
    } // setupUi

    void retranslateUi(QWidget *SchedulerView)
    {
        btnRefInt->setText(QCoreApplication::translate("SchedulerView", "Refresh", nullptr));
        btnAddInt->setText(QCoreApplication::translate("SchedulerView", "Add Interval", nullptr));
        btnDelInt->setText(QCoreApplication::translate("SchedulerView", "Delete Interval", nullptr));
        QTableWidgetItem *___qtablewidgetitem = intervalsTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("SchedulerView", "Name", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = intervalsTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("SchedulerView", "Interval", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(intervalsTab), QCoreApplication::translate("SchedulerView", "Intervals", nullptr));
        btnRefAct->setText(QCoreApplication::translate("SchedulerView", "Refresh", nullptr));
        btnAddAct->setText(QCoreApplication::translate("SchedulerView", "Add Action", nullptr));
        btnDelAct->setText(QCoreApplication::translate("SchedulerView", "Delete Action", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = actionsTable->horizontalHeaderItem(0);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("SchedulerView", "Name", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = actionsTable->horizontalHeaderItem(1);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("SchedulerView", "Target", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = actionsTable->horizontalHeaderItem(2);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("SchedulerView", "Interval", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(actionsTab), QCoreApplication::translate("SchedulerView", "Interval Actions", nullptr));
        (void)SchedulerView;
    } // retranslateUi

};

namespace Ui {
    class SchedulerView: public Ui_SchedulerView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCHEDULERVIEW_H
