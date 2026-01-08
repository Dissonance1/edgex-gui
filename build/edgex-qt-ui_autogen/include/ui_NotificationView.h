/********************************************************************************
** Form generated from reading UI file 'NotificationView.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NOTIFICATIONVIEW_H
#define UI_NOTIFICATIONVIEW_H

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

class Ui_NotificationView
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *notificationsTab;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *nToolbar;
    QPushButton *btnRefresh;
    QPushButton *btnCleanup;
    QSpacerItem *horizontalSpacer;
    QTableWidget *table;
    QWidget *subscriptionsTab;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *sToolbar;
    QPushButton *btnRefreshSub;
    QPushButton *btnAddSub;
    QPushButton *btnDeleteSub;
    QSpacerItem *horizontalSpacer_2;
    QTableWidget *subTable;

    void setupUi(QWidget *NotificationView)
    {
        if (NotificationView->objectName().isEmpty())
            NotificationView->setObjectName("NotificationView");
        NotificationView->resize(800, 600);
        verticalLayout = new QVBoxLayout(NotificationView);
        verticalLayout->setObjectName("verticalLayout");
        tabWidget = new QTabWidget(NotificationView);
        tabWidget->setObjectName("tabWidget");
        notificationsTab = new QWidget();
        notificationsTab->setObjectName("notificationsTab");
        verticalLayout_2 = new QVBoxLayout(notificationsTab);
        verticalLayout_2->setObjectName("verticalLayout_2");
        nToolbar = new QHBoxLayout();
        nToolbar->setObjectName("nToolbar");
        btnRefresh = new QPushButton(notificationsTab);
        btnRefresh->setObjectName("btnRefresh");

        nToolbar->addWidget(btnRefresh);

        btnCleanup = new QPushButton(notificationsTab);
        btnCleanup->setObjectName("btnCleanup");

        nToolbar->addWidget(btnCleanup);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        nToolbar->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(nToolbar);

        table = new QTableWidget(notificationsTab);
        if (table->columnCount() < 5)
            table->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        table->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        table->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        table->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        table->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        table->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        table->setObjectName("table");
        table->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout_2->addWidget(table);

        tabWidget->addTab(notificationsTab, QString());
        subscriptionsTab = new QWidget();
        subscriptionsTab->setObjectName("subscriptionsTab");
        verticalLayout_3 = new QVBoxLayout(subscriptionsTab);
        verticalLayout_3->setObjectName("verticalLayout_3");
        sToolbar = new QHBoxLayout();
        sToolbar->setObjectName("sToolbar");
        btnRefreshSub = new QPushButton(subscriptionsTab);
        btnRefreshSub->setObjectName("btnRefreshSub");

        sToolbar->addWidget(btnRefreshSub);

        btnAddSub = new QPushButton(subscriptionsTab);
        btnAddSub->setObjectName("btnAddSub");

        sToolbar->addWidget(btnAddSub);

        btnDeleteSub = new QPushButton(subscriptionsTab);
        btnDeleteSub->setObjectName("btnDeleteSub");

        sToolbar->addWidget(btnDeleteSub);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        sToolbar->addItem(horizontalSpacer_2);


        verticalLayout_3->addLayout(sToolbar);

        subTable = new QTableWidget(subscriptionsTab);
        if (subTable->columnCount() < 5)
            subTable->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        subTable->setHorizontalHeaderItem(0, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        subTable->setHorizontalHeaderItem(1, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        subTable->setHorizontalHeaderItem(2, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        subTable->setHorizontalHeaderItem(3, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        subTable->setHorizontalHeaderItem(4, __qtablewidgetitem9);
        subTable->setObjectName("subTable");
        subTable->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout_3->addWidget(subTable);

        tabWidget->addTab(subscriptionsTab, QString());

        verticalLayout->addWidget(tabWidget);


        retranslateUi(NotificationView);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(NotificationView);
    } // setupUi

    void retranslateUi(QWidget *NotificationView)
    {
        btnRefresh->setText(QCoreApplication::translate("NotificationView", "Refresh", nullptr));
        btnCleanup->setText(QCoreApplication::translate("NotificationView", "Cleanup All", nullptr));
        QTableWidgetItem *___qtablewidgetitem = table->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("NotificationView", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = table->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("NotificationView", "Category", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = table->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("NotificationView", "Content", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = table->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("NotificationView", "Urgency", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = table->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("NotificationView", "Created", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(notificationsTab), QCoreApplication::translate("NotificationView", "Notifications", nullptr));
        btnRefreshSub->setText(QCoreApplication::translate("NotificationView", "Refresh", nullptr));
        btnAddSub->setText(QCoreApplication::translate("NotificationView", "Add Subscription", nullptr));
        btnDeleteSub->setText(QCoreApplication::translate("NotificationView", "Delete", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = subTable->horizontalHeaderItem(0);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("NotificationView", "Name", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = subTable->horizontalHeaderItem(1);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("NotificationView", "Receiver", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = subTable->horizontalHeaderItem(2);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("NotificationView", "Categories", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = subTable->horizontalHeaderItem(3);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("NotificationView", "Labels", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = subTable->horizontalHeaderItem(4);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("NotificationView", "Admin State", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(subscriptionsTab), QCoreApplication::translate("NotificationView", "Subscriptions", nullptr));
        (void)NotificationView;
    } // retranslateUi

};

namespace Ui {
    class NotificationView: public Ui_NotificationView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NOTIFICATIONVIEW_H
