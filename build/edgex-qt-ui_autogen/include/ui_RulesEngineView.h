/********************************************************************************
** Form generated from reading UI file 'RulesEngineView.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RULESENGINEVIEW_H
#define UI_RULESENGINEVIEW_H

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

class Ui_RulesEngineView
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *streamsTab;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *sToolbar;
    QPushButton *btnRefreshStreams;
    QPushButton *btnAddStream;
    QPushButton *btnDeleteStream;
    QSpacerItem *horizontalSpacer;
    QTableWidget *streamsTable;
    QWidget *rulesTab;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *rToolbar;
    QPushButton *btnRefreshRules;
    QPushButton *btnAddRule;
    QPushButton *btnStartRule;
    QPushButton *btnStopRule;
    QPushButton *btnViewSql;
    QPushButton *btnMetrics;
    QPushButton *btnDeleteRule;
    QSpacerItem *horizontalSpacer_2;
    QTableWidget *rulesTable;

    void setupUi(QWidget *RulesEngineView)
    {
        if (RulesEngineView->objectName().isEmpty())
            RulesEngineView->setObjectName("RulesEngineView");
        RulesEngineView->resize(800, 600);
        verticalLayout = new QVBoxLayout(RulesEngineView);
        verticalLayout->setObjectName("verticalLayout");
        tabWidget = new QTabWidget(RulesEngineView);
        tabWidget->setObjectName("tabWidget");
        streamsTab = new QWidget();
        streamsTab->setObjectName("streamsTab");
        verticalLayout_2 = new QVBoxLayout(streamsTab);
        verticalLayout_2->setObjectName("verticalLayout_2");
        sToolbar = new QHBoxLayout();
        sToolbar->setObjectName("sToolbar");
        btnRefreshStreams = new QPushButton(streamsTab);
        btnRefreshStreams->setObjectName("btnRefreshStreams");

        sToolbar->addWidget(btnRefreshStreams);

        btnAddStream = new QPushButton(streamsTab);
        btnAddStream->setObjectName("btnAddStream");

        sToolbar->addWidget(btnAddStream);

        btnDeleteStream = new QPushButton(streamsTab);
        btnDeleteStream->setObjectName("btnDeleteStream");

        sToolbar->addWidget(btnDeleteStream);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        sToolbar->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(sToolbar);

        streamsTable = new QTableWidget(streamsTab);
        if (streamsTable->columnCount() < 1)
            streamsTable->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        streamsTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        streamsTable->setObjectName("streamsTable");
        streamsTable->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout_2->addWidget(streamsTable);

        tabWidget->addTab(streamsTab, QString());
        rulesTab = new QWidget();
        rulesTab->setObjectName("rulesTab");
        verticalLayout_3 = new QVBoxLayout(rulesTab);
        verticalLayout_3->setObjectName("verticalLayout_3");
        rToolbar = new QHBoxLayout();
        rToolbar->setObjectName("rToolbar");
        btnRefreshRules = new QPushButton(rulesTab);
        btnRefreshRules->setObjectName("btnRefreshRules");

        rToolbar->addWidget(btnRefreshRules);

        btnAddRule = new QPushButton(rulesTab);
        btnAddRule->setObjectName("btnAddRule");

        rToolbar->addWidget(btnAddRule);

        btnStartRule = new QPushButton(rulesTab);
        btnStartRule->setObjectName("btnStartRule");

        rToolbar->addWidget(btnStartRule);

        btnStopRule = new QPushButton(rulesTab);
        btnStopRule->setObjectName("btnStopRule");

        rToolbar->addWidget(btnStopRule);

        btnViewSql = new QPushButton(rulesTab);
        btnViewSql->setObjectName("btnViewSql");

        rToolbar->addWidget(btnViewSql);

        btnMetrics = new QPushButton(rulesTab);
        btnMetrics->setObjectName("btnMetrics");

        rToolbar->addWidget(btnMetrics);

        btnDeleteRule = new QPushButton(rulesTab);
        btnDeleteRule->setObjectName("btnDeleteRule");

        rToolbar->addWidget(btnDeleteRule);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        rToolbar->addItem(horizontalSpacer_2);


        verticalLayout_3->addLayout(rToolbar);

        rulesTable = new QTableWidget(rulesTab);
        if (rulesTable->columnCount() < 4)
            rulesTable->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        rulesTable->setHorizontalHeaderItem(0, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        rulesTable->setHorizontalHeaderItem(1, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        rulesTable->setHorizontalHeaderItem(2, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        rulesTable->setHorizontalHeaderItem(3, __qtablewidgetitem4);
        rulesTable->setObjectName("rulesTable");
        rulesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout_3->addWidget(rulesTable);

        tabWidget->addTab(rulesTab, QString());

        verticalLayout->addWidget(tabWidget);


        retranslateUi(RulesEngineView);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(RulesEngineView);
    } // setupUi

    void retranslateUi(QWidget *RulesEngineView)
    {
        btnRefreshStreams->setText(QCoreApplication::translate("RulesEngineView", "Refresh", nullptr));
        btnAddStream->setText(QCoreApplication::translate("RulesEngineView", "Add Stream", nullptr));
        btnDeleteStream->setText(QCoreApplication::translate("RulesEngineView", "Delete Stream", nullptr));
        btnDeleteStream->setStyleSheet(QCoreApplication::translate("RulesEngineView", "background-color: #e74c3c; color: white;", nullptr));
        QTableWidgetItem *___qtablewidgetitem = streamsTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("RulesEngineView", "Stream Name", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(streamsTab), QCoreApplication::translate("RulesEngineView", "Streams", nullptr));
        btnRefreshRules->setText(QCoreApplication::translate("RulesEngineView", "Refresh", nullptr));
        btnAddRule->setText(QCoreApplication::translate("RulesEngineView", "Add Rule", nullptr));
        btnStartRule->setText(QCoreApplication::translate("RulesEngineView", "Start", nullptr));
        btnStopRule->setText(QCoreApplication::translate("RulesEngineView", "Stop", nullptr));
        btnViewSql->setText(QCoreApplication::translate("RulesEngineView", "View SQL", nullptr));
        btnMetrics->setText(QCoreApplication::translate("RulesEngineView", "Metrics", nullptr));
        btnDeleteRule->setText(QCoreApplication::translate("RulesEngineView", "Delete Rule", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = rulesTable->horizontalHeaderItem(0);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("RulesEngineView", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = rulesTable->horizontalHeaderItem(1);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("RulesEngineView", "Status", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = rulesTable->horizontalHeaderItem(2);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("RulesEngineView", "Actions", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = rulesTable->horizontalHeaderItem(3);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("RulesEngineView", "Metrics Summary", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(rulesTab), QCoreApplication::translate("RulesEngineView", "Rules", nullptr));
        (void)RulesEngineView;
    } // retranslateUi

};

namespace Ui {
    class RulesEngineView: public Ui_RulesEngineView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RULESENGINEVIEW_H
