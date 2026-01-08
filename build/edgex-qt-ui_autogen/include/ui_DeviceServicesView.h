/********************************************************************************
** Form generated from reading UI file 'DeviceServicesView.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEVICESERVICESVIEW_H
#define UI_DEVICESERVICESVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DeviceServicesView
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *toolbar;
    QPushButton *btnRefresh;
    QPushButton *btnEdit;
    QPushButton *btnDelete;
    QPushButton *btnLock;
    QPushButton *btnUnlock;
    QLabel *labelSearch;
    QLineEdit *searchEdit;
    QSpacerItem *horizontalSpacer;
    QTableView *tableView;

    void setupUi(QWidget *DeviceServicesView)
    {
        if (DeviceServicesView->objectName().isEmpty())
            DeviceServicesView->setObjectName("DeviceServicesView");
        DeviceServicesView->resize(800, 600);
        verticalLayout = new QVBoxLayout(DeviceServicesView);
        verticalLayout->setObjectName("verticalLayout");
        toolbar = new QHBoxLayout();
        toolbar->setObjectName("toolbar");
        btnRefresh = new QPushButton(DeviceServicesView);
        btnRefresh->setObjectName("btnRefresh");

        toolbar->addWidget(btnRefresh);

        btnEdit = new QPushButton(DeviceServicesView);
        btnEdit->setObjectName("btnEdit");

        toolbar->addWidget(btnEdit);

        btnDelete = new QPushButton(DeviceServicesView);
        btnDelete->setObjectName("btnDelete");

        toolbar->addWidget(btnDelete);

        btnLock = new QPushButton(DeviceServicesView);
        btnLock->setObjectName("btnLock");

        toolbar->addWidget(btnLock);

        btnUnlock = new QPushButton(DeviceServicesView);
        btnUnlock->setObjectName("btnUnlock");

        toolbar->addWidget(btnUnlock);

        labelSearch = new QLabel(DeviceServicesView);
        labelSearch->setObjectName("labelSearch");

        toolbar->addWidget(labelSearch);

        searchEdit = new QLineEdit(DeviceServicesView);
        searchEdit->setObjectName("searchEdit");

        toolbar->addWidget(searchEdit);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        toolbar->addItem(horizontalSpacer);


        verticalLayout->addLayout(toolbar);

        tableView = new QTableView(DeviceServicesView);
        tableView->setObjectName("tableView");
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->horizontalHeader()->setStretchLastSection(true);

        verticalLayout->addWidget(tableView);


        retranslateUi(DeviceServicesView);

        QMetaObject::connectSlotsByName(DeviceServicesView);
    } // setupUi

    void retranslateUi(QWidget *DeviceServicesView)
    {
        btnRefresh->setText(QCoreApplication::translate("DeviceServicesView", "Refresh", nullptr));
        btnEdit->setText(QCoreApplication::translate("DeviceServicesView", "Edit", nullptr));
        btnDelete->setText(QCoreApplication::translate("DeviceServicesView", "Delete", nullptr));
        btnLock->setText(QCoreApplication::translate("DeviceServicesView", "Lock", nullptr));
        btnUnlock->setText(QCoreApplication::translate("DeviceServicesView", "Unlock", nullptr));
        labelSearch->setText(QCoreApplication::translate("DeviceServicesView", "Search:", nullptr));
        searchEdit->setPlaceholderText(QCoreApplication::translate("DeviceServicesView", "Search services...", nullptr));
        (void)DeviceServicesView;
    } // retranslateUi

};

namespace Ui {
    class DeviceServicesView: public Ui_DeviceServicesView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEVICESERVICESVIEW_H
