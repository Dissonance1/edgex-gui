#include "ProfilesView.h"
#include "ui_ProfilesView.h"
#include <QHeaderView>
#include <QMessageBox>
#include "ConfigManager.h"
#include "AddProfileDialog.h"

ProfilesView::ProfilesView(QWidget *parent)
    : QWidget(parent), ui(new Ui::ProfilesView), m_model(new ProfilesModel(this)), m_client(new MetadataClient(this))
{
    ui->setupUi(this);

    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterKeyColumn(-1);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    ui->tableView->setModel(m_proxyModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Connections
    connect(m_client, &MetadataClient::deviceProfilesReceived, this, &ProfilesView::onProfilesReceived);
    connect(m_client, &MetadataClient::operationCompleted, this, &ProfilesView::onOperationCompleted);
    
    connect(ui->btnRefresh, &QPushButton::clicked, this, &ProfilesView::refresh);
    connect(ui->btnAdd, &QPushButton::clicked, this, &ProfilesView::onAddProfile);
    connect(ui->btnDelete, &QPushButton::clicked, this, [this]() {
        QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
        if (selected.isEmpty()) return;
        
        QModelIndex sourceIndex = m_proxyModel->mapToSource(selected.first());
        QString profileName = m_model->data(m_model->index(sourceIndex.row(), 1)).toString();
        if (QMessageBox::question(this, "Confirm Delete", 
                                  "Are you sure you want to delete profile: " + profileName + "?") == QMessageBox::Yes) {
            m_client->setBaseUrl(ConfigManager::instance().metadataUrl());
            m_client->deleteProfile(profileName);
        }
    });

    connect(ui->searchEdit, &QLineEdit::textChanged, this, &ProfilesView::onSearch);

    refresh();
}

ProfilesView::~ProfilesView()
{
    delete ui;
}

void ProfilesView::refresh()
{
    m_client->setBaseUrl(ConfigManager::instance().metadataUrl());
    m_client->fetchDeviceProfiles();
}

void ProfilesView::onProfilesReceived(const QJsonArray &profiles)
{
    m_model->setProfiles(profiles);
}

void ProfilesView::onAddProfile()
{
    AddProfileDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        m_client->setBaseUrl(ConfigManager::instance().metadataUrl());
        m_client->addProfileFromYaml(dlg.yamlContent());
    }
}

void ProfilesView::onOperationCompleted(bool success, const QString &message)
{
    if (success) {
        QMessageBox::information(this, "Success", message);
        refresh();
    } else {
        QMessageBox::warning(this, "Error", message);
    }
}

void ProfilesView::onSearch(const QString &text)
{
    m_proxyModel->setFilterFixedString(text);
}
