#include "EventsReadingsView.h"
#include "ui_EventsReadingsView.h"
#include <QHeaderView>
#include <QMessageBox>
#include "ConfigManager.h"

EventsReadingsView::EventsReadingsView(QWidget *parent)
    : QWidget(parent), 
      ui(new Ui::EventsReadingsView),
      m_eventsModel(new EventsModel(this)), 
      m_readingsModel(new ReadingsModel(this)),
      m_client(new CoreDataClient(this))
{
    ui->setupUi(this);

    m_eventsProxy = new QSortFilterProxyModel(this);
    m_eventsProxy->setSourceModel(m_eventsModel);
    m_eventsProxy->setFilterKeyColumn(-1);
    m_eventsProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->eventsTable->setModel(m_eventsProxy);
    ui->eventsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    ui->readingsTable->setModel(m_readingsModel);
    ui->readingsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(m_client, &CoreDataClient::eventsReceived, this, &EventsReadingsView::onEventsReceived);
    connect(m_client, &CoreDataClient::operationCompleted, this, &EventsReadingsView::onOperationCompleted);
    
    connect(ui->eventsTable, &QTableView::clicked, this, &EventsReadingsView::onEventSelected);
    connect(ui->btnRefresh, &QPushButton::clicked, this, &EventsReadingsView::refresh);
    connect(ui->searchEdit, &QLineEdit::textChanged, this, &EventsReadingsView::onSearchEvents);
    
    refresh();
}

EventsReadingsView::~EventsReadingsView()
{
    delete ui;
}

void EventsReadingsView::refresh()
{
    m_client->setBaseUrl(ConfigManager::instance().coreDataUrl());
    m_client->fetchEvents();
}

void EventsReadingsView::onEventsReceived(const QJsonArray &events)
{
    m_currentEvents = events;
    m_eventsModel->setEvents(events);
}

void EventsReadingsView::onEventSelected(const QModelIndex &index)
{
    if (!index.isValid()) return;
    QModelIndex sourceIndex = m_eventsProxy->mapToSource(index);
    QJsonObject event = m_currentEvents[sourceIndex.row()].toObject();
    m_readingsModel->setReadings(event["readings"].toArray());
}

void EventsReadingsView::onSearchEvents(const QString &text)
{
    m_eventsProxy->setFilterFixedString(text);
}

void EventsReadingsView::onOperationCompleted(bool success, const QString &message)
{
    if (success) {
        QMessageBox::information(this, "Success", message);
        refresh();
    } else {
        QMessageBox::warning(this, "Error", message);
    }
}
