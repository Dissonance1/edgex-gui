/********************************************************************************
** Form generated from reading UI file 'ProfilesView.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROFILESVIEW_H
#define UI_PROFILESVIEW_H

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

class Ui_ProfilesView
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *toolbar;
    QPushButton *btnRefresh;
    QPushButton *btnAdd;
    QPushButton *btnDelete;
    QLabel *labelSearch;
    QLineEdit *searchEdit;
    QSpacerItem *horizontalSpacer;
    QTableView *tableView;

    void setupUi(QWidget *ProfilesView)
    {
        if (ProfilesView->objectName().isEmpty())
            ProfilesView->setObjectName("ProfilesView");
        ProfilesView->resize(800, 600);
        verticalLayout = new QVBoxLayout(ProfilesView);
        verticalLayout->setObjectName("verticalLayout");
        toolbar = new QHBoxLayout();
        toolbar->setObjectName("toolbar");
        btnRefresh = new QPushButton(ProfilesView);
        btnRefresh->setObjectName("btnRefresh");

        toolbar->addWidget(btnRefresh);

        btnAdd = new QPushButton(ProfilesView);
        btnAdd->setObjectName("btnAdd");

        toolbar->addWidget(btnAdd);

        btnDelete = new QPushButton(ProfilesView);
        btnDelete->setObjectName("btnDelete");

        toolbar->addWidget(btnDelete);

        labelSearch = new QLabel(ProfilesView);
        labelSearch->setObjectName("labelSearch");

        toolbar->addWidget(labelSearch);

        searchEdit = new QLineEdit(ProfilesView);
        searchEdit->setObjectName("searchEdit");

        toolbar->addWidget(searchEdit);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        toolbar->addItem(horizontalSpacer);


        verticalLayout->addLayout(toolbar);

        tableView = new QTableView(ProfilesView);
        tableView->setObjectName("tableView");
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->horizontalHeader()->setStretchLastSection(true);

        verticalLayout->addWidget(tableView);


        retranslateUi(ProfilesView);

        QMetaObject::connectSlotsByName(ProfilesView);
    } // setupUi

    void retranslateUi(QWidget *ProfilesView)
    {
        btnRefresh->setText(QCoreApplication::translate("ProfilesView", "Refresh", nullptr));
        btnAdd->setText(QCoreApplication::translate("ProfilesView", "Add Profile", nullptr));
        btnDelete->setText(QCoreApplication::translate("ProfilesView", "Delete Profile", nullptr));
        labelSearch->setText(QCoreApplication::translate("ProfilesView", "Search:", nullptr));
        searchEdit->setPlaceholderText(QCoreApplication::translate("ProfilesView", "Search profiles...", nullptr));
        (void)ProfilesView;
    } // retranslateUi

};

namespace Ui {
    class ProfilesView: public Ui_ProfilesView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROFILESVIEW_H
