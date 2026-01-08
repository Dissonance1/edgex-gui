/********************************************************************************
** Form generated from reading UI file 'EventsReadingsView.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EVENTSREADINGSVIEW_H
#define UI_EVENTSREADINGSVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_EventsReadingsView
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *toolbar;
    QPushButton *btnRefresh;
    QLabel *labelSearch;
    QLineEdit *searchEdit;
    QSpacerItem *horizontalSpacer;
    QSplitter *splitter;
    QTableView *eventsTable;
    QTableView *readingsTable;

    void setupUi(QWidget *EventsReadingsView)
    {
        if (EventsReadingsView->objectName().isEmpty())
            EventsReadingsView->setObjectName("EventsReadingsView");
        EventsReadingsView->resize(800, 600);
        verticalLayout = new QVBoxLayout(EventsReadingsView);
        verticalLayout->setObjectName("verticalLayout");
        toolbar = new QHBoxLayout();
        toolbar->setObjectName("toolbar");
        btnRefresh = new QPushButton(EventsReadingsView);
        btnRefresh->setObjectName("btnRefresh");

        toolbar->addWidget(btnRefresh);

        labelSearch = new QLabel(EventsReadingsView);
        labelSearch->setObjectName("labelSearch");

        toolbar->addWidget(labelSearch);

        searchEdit = new QLineEdit(EventsReadingsView);
        searchEdit->setObjectName("searchEdit");

        toolbar->addWidget(searchEdit);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        toolbar->addItem(horizontalSpacer);


        verticalLayout->addLayout(toolbar);

        splitter = new QSplitter(EventsReadingsView);
        splitter->setObjectName("splitter");
        splitter->setOrientation(Qt::Vertical);
        eventsTable = new QTableView(splitter);
        eventsTable->setObjectName("eventsTable");
        eventsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        splitter->addWidget(eventsTable);
        eventsTable->horizontalHeader()->setStretchLastSection(true);
        readingsTable = new QTableView(splitter);
        readingsTable->setObjectName("readingsTable");
        readingsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        splitter->addWidget(readingsTable);
        readingsTable->horizontalHeader()->setStretchLastSection(true);

        verticalLayout->addWidget(splitter);


        retranslateUi(EventsReadingsView);

        QMetaObject::connectSlotsByName(EventsReadingsView);
    } // setupUi

    void retranslateUi(QWidget *EventsReadingsView)
    {
        btnRefresh->setText(QCoreApplication::translate("EventsReadingsView", "Refresh Recent", nullptr));
        labelSearch->setText(QCoreApplication::translate("EventsReadingsView", "Search:", nullptr));
        searchEdit->setPlaceholderText(QCoreApplication::translate("EventsReadingsView", "Search events...", nullptr));
        (void)EventsReadingsView;
    } // retranslateUi

};

namespace Ui {
    class EventsReadingsView: public Ui_EventsReadingsView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EVENTSREADINGSVIEW_H
