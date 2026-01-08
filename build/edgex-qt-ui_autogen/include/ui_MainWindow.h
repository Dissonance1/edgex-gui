/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter;
    QWidget *navContainer;
    QVBoxLayout *verticalLayout;
    QLabel *logoLabel;
    QListWidget *navPanel;
    QStackedWidget *centralStack;
    QMenuBar *menuBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1280, 850);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        splitter = new QSplitter(centralWidget);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Horizontal);
        navContainer = new QWidget(splitter);
        navContainer->setObjectName("navContainer");
        navContainer->setMinimumSize(QSize(260, 0));
        navContainer->setMaximumSize(QSize(260, 16777215));
        navContainer->setStyleSheet(QString::fromUtf8("background-color: #FFFDF2; border-right: 1px solid #E0E0E0;"));
        verticalLayout = new QVBoxLayout(navContainer);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 20);
        logoLabel = new QLabel(navContainer);
        logoLabel->setObjectName("logoLabel");
        logoLabel->setMaximumSize(QSize(16777215, 100));
        logoLabel->setPixmap(QPixmap(QString::fromUtf8(":/resources/honeycomblogo.png")));
        logoLabel->setScaledContents(true);
        logoLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        logoLabel->setMargin(15);

        verticalLayout->addWidget(logoLabel, 0, Qt::AlignLeft);

        navPanel = new QListWidget(navContainer);
        QFont font;
        font.setPointSize(14);
        QListWidgetItem *__qlistwidgetitem = new QListWidgetItem(navPanel);
        __qlistwidgetitem->setFont(font);
        QListWidgetItem *__qlistwidgetitem1 = new QListWidgetItem(navPanel);
        __qlistwidgetitem1->setFont(font);
        QListWidgetItem *__qlistwidgetitem2 = new QListWidgetItem(navPanel);
        __qlistwidgetitem2->setFont(font);
        QListWidgetItem *__qlistwidgetitem3 = new QListWidgetItem(navPanel);
        __qlistwidgetitem3->setFont(font);
        QListWidgetItem *__qlistwidgetitem4 = new QListWidgetItem(navPanel);
        __qlistwidgetitem4->setFont(font);
        QListWidgetItem *__qlistwidgetitem5 = new QListWidgetItem(navPanel);
        __qlistwidgetitem5->setFont(font);
        QListWidgetItem *__qlistwidgetitem6 = new QListWidgetItem(navPanel);
        __qlistwidgetitem6->setFont(font);
        QListWidgetItem *__qlistwidgetitem7 = new QListWidgetItem(navPanel);
        __qlistwidgetitem7->setFont(font);
        navPanel->setObjectName("navPanel");
        navPanel->setFrameShape(QFrame::NoFrame);
        navPanel->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        verticalLayout->addWidget(navPanel);

        splitter->addWidget(navContainer);
        centralStack = new QStackedWidget(splitter);
        centralStack->setObjectName("centralStack");
        splitter->addWidget(centralStack);

        horizontalLayout->addWidget(splitter);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 1280, 19));
        MainWindow->setMenuBar(menuBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Honeycomb edge", nullptr));
        logoLabel->setText(QString());

        const bool __sortingEnabled = navPanel->isSortingEnabled();
        navPanel->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = navPanel->item(0);
        ___qlistwidgetitem->setText(QCoreApplication::translate("MainWindow", "Dashboard", nullptr));
        QListWidgetItem *___qlistwidgetitem1 = navPanel->item(1);
        ___qlistwidgetitem1->setText(QCoreApplication::translate("MainWindow", "System Status", nullptr));
        QListWidgetItem *___qlistwidgetitem2 = navPanel->item(2);
        ___qlistwidgetitem2->setText(QCoreApplication::translate("MainWindow", "Device Services", nullptr));
        QListWidgetItem *___qlistwidgetitem3 = navPanel->item(3);
        ___qlistwidgetitem3->setText(QCoreApplication::translate("MainWindow", "Device Profiles", nullptr));
        QListWidgetItem *___qlistwidgetitem4 = navPanel->item(4);
        ___qlistwidgetitem4->setText(QCoreApplication::translate("MainWindow", "Devices", nullptr));
        QListWidgetItem *___qlistwidgetitem5 = navPanel->item(5);
        ___qlistwidgetitem5->setText(QCoreApplication::translate("MainWindow", "Events & Readings", nullptr));
        QListWidgetItem *___qlistwidgetitem6 = navPanel->item(6);
        ___qlistwidgetitem6->setText(QCoreApplication::translate("MainWindow", "Rules Engine", nullptr));
        QListWidgetItem *___qlistwidgetitem7 = navPanel->item(7);
        ___qlistwidgetitem7->setText(QCoreApplication::translate("MainWindow", "Notifications", nullptr));
        navPanel->setSortingEnabled(__sortingEnabled);

    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
