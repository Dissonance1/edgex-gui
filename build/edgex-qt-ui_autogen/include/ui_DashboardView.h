/********************************************************************************
** Form generated from reading UI file 'DashboardView.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DASHBOARDVIEW_H
#define UI_DASHBOARDVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DashboardView
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *headerLayout;
    QLabel *titleLabel;
    QSpacerItem *headerSpacer;
    QPushButton *refreshButton;
    QGridLayout *cardGrid;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *DashboardView)
    {
        if (DashboardView->objectName().isEmpty())
            DashboardView->setObjectName("DashboardView");
        DashboardView->resize(855, 600);
        verticalLayout = new QVBoxLayout(DashboardView);
        verticalLayout->setSpacing(20);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(20, 20, 20, 20);
        headerLayout = new QHBoxLayout();
        headerLayout->setObjectName("headerLayout");
        titleLabel = new QLabel(DashboardView);
        titleLabel->setObjectName("titleLabel");

        headerLayout->addWidget(titleLabel);

        headerSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        headerLayout->addItem(headerSpacer);

        refreshButton = new QPushButton(DashboardView);
        refreshButton->setObjectName("refreshButton");

        headerLayout->addWidget(refreshButton);


        verticalLayout->addLayout(headerLayout);

        cardGrid = new QGridLayout();
        cardGrid->setSpacing(20);
        cardGrid->setObjectName("cardGrid");

        verticalLayout->addLayout(cardGrid);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(DashboardView);

        QMetaObject::connectSlotsByName(DashboardView);
    } // setupUi

    void retranslateUi(QWidget *DashboardView)
    {
        titleLabel->setStyleSheet(QCoreApplication::translate("DashboardView", "font-size: 18px; font-weight: bold;", nullptr));
        titleLabel->setText(QCoreApplication::translate("DashboardView", "Honeycomb Edge System", nullptr));
        refreshButton->setText(QCoreApplication::translate("DashboardView", "Refresh All", nullptr));
        (void)DashboardView;
    } // retranslateUi

};

namespace Ui {
    class DashboardView: public Ui_DashboardView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DASHBOARDVIEW_H
