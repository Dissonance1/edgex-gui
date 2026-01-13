/********************************************************************************
** Form generated from reading UI file 'AppServicesView.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_APPSERVICESVIEW_H
#define UI_APPSERVICESVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AppServicesView
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *toolbar;
    QPushButton *btnRefresh;
    QPushButton *btnConfig;
    QPushButton *btnTemplate;
    QLabel *labelSearch;
    QLineEdit *searchEdit;
    QSpacerItem *horizontalSpacer;
    QTableWidget *tableWidget;

    void setupUi(QWidget *AppServicesView)
    {
        if (AppServicesView->objectName().isEmpty())
            AppServicesView->setObjectName("AppServicesView");
        AppServicesView->resize(800, 600);
        verticalLayout = new QVBoxLayout(AppServicesView);
        verticalLayout->setObjectName("verticalLayout");
        toolbar = new QHBoxLayout();
        toolbar->setObjectName("toolbar");
        btnRefresh = new QPushButton(AppServicesView);
        btnRefresh->setObjectName("btnRefresh");

        toolbar->addWidget(btnRefresh);

        btnConfig = new QPushButton(AppServicesView);
        btnConfig->setObjectName("btnConfig");

        toolbar->addWidget(btnConfig);

        btnTemplate = new QPushButton(AppServicesView);
        btnTemplate->setObjectName("btnTemplate");

        toolbar->addWidget(btnTemplate);

        labelSearch = new QLabel(AppServicesView);
        labelSearch->setObjectName("labelSearch");

        toolbar->addWidget(labelSearch);

        searchEdit = new QLineEdit(AppServicesView);
        searchEdit->setObjectName("searchEdit");

        toolbar->addWidget(searchEdit);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        toolbar->addItem(horizontalSpacer);


        verticalLayout->addLayout(toolbar);

        tableWidget = new QTableWidget(AppServicesView);
        if (tableWidget->columnCount() < 3)
            tableWidget->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        tableWidget->setObjectName("tableWidget");
        tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout->addWidget(tableWidget);


        retranslateUi(AppServicesView);

        QMetaObject::connectSlotsByName(AppServicesView);
    } // setupUi

    void retranslateUi(QWidget *AppServicesView)
    {
        btnRefresh->setText(QCoreApplication::translate("AppServicesView", "Refresh", nullptr));
        btnConfig->setText(QCoreApplication::translate("AppServicesView", "View Config", nullptr));
        btnTemplate->setText(QCoreApplication::translate("AppServicesView", "Create from Template", nullptr));
        labelSearch->setText(QCoreApplication::translate("AppServicesView", "Search:", nullptr));
        searchEdit->setPlaceholderText(QCoreApplication::translate("AppServicesView", "Search app services...", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("AppServicesView", "Name", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("AppServicesView", "Admin State", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("AppServicesView", "Operating State", nullptr));
        (void)AppServicesView;
    } // retranslateUi

};

namespace Ui {
    class AppServicesView: public Ui_AppServicesView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_APPSERVICESVIEW_H
