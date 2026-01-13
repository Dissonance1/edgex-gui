/********************************************************************************
** Form generated from reading UI file 'DevicesView.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEVICESVIEW_H
#define UI_DEVICESVIEW_H

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

class Ui_DevicesView
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *topLayout;
    QLabel *titleLabel;
    QSpacerItem *horizontalSpacer;
    QLineEdit *searchEdit;
    QPushButton *refreshButton;
    QPushButton *addButton;
    QTableView *tableView;

    void setupUi(QWidget *DevicesView)
    {
        if (DevicesView->objectName().isEmpty())
            DevicesView->setObjectName("DevicesView");
        DevicesView->resize(800, 600);
        verticalLayout = new QVBoxLayout(DevicesView);
        verticalLayout->setSpacing(10);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(10, 10, 10, 10);
        topLayout = new QHBoxLayout();
        topLayout->setObjectName("topLayout");
        titleLabel = new QLabel(DevicesView);
        titleLabel->setObjectName("titleLabel");
        titleLabel->setStyleSheet(QString::fromUtf8("font-size: 18px; font-weight: bold;"));

        topLayout->addWidget(titleLabel);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        topLayout->addItem(horizontalSpacer);

        searchEdit = new QLineEdit(DevicesView);
        searchEdit->setObjectName("searchEdit");

        topLayout->addWidget(searchEdit);

        refreshButton = new QPushButton(DevicesView);
        refreshButton->setObjectName("refreshButton");

        topLayout->addWidget(refreshButton);

        addButton = new QPushButton(DevicesView);
        addButton->setObjectName("addButton");

        topLayout->addWidget(addButton);


        verticalLayout->addLayout(topLayout);

        tableView = new QTableView(DevicesView);
        tableView->setObjectName("tableView");
        tableView->setAlternatingRowColors(true);
        tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->horizontalHeader()->setStretchLastSection(true);

        verticalLayout->addWidget(tableView);


        retranslateUi(DevicesView);

        QMetaObject::connectSlotsByName(DevicesView);
    } // setupUi

    void retranslateUi(QWidget *DevicesView)
    {
        titleLabel->setText(QCoreApplication::translate("DevicesView", "Devices", nullptr));
        searchEdit->setPlaceholderText(QCoreApplication::translate("DevicesView", "Search Devices...", nullptr));
        refreshButton->setText(QCoreApplication::translate("DevicesView", "Refresh", nullptr));
        addButton->setText(QCoreApplication::translate("DevicesView", "Add Device", nullptr));
        (void)DevicesView;
    } // retranslateUi

};

namespace Ui {
    class DevicesView: public Ui_DevicesView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEVICESVIEW_H
