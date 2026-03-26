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
    connect(ui->btnEdit, &QPushButton::clicked, this, &ProfilesView::onEditProfile);
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

static QString jsonToYaml(const QJsonValue &value, int indent = 0) {
    QString yaml;
    QString space = QString(indent, ' ');
    
    if (value.isObject()) {
        QJsonObject obj = value.toObject();
        QStringList keys = obj.keys();
        for (const QString &key : keys) {
            QJsonValue v = obj[key];
            if (v.isNull() || v.isUndefined()) continue;
            
            if (v.isObject()) {
                yaml += space + key + ":\n" + jsonToYaml(v, indent + 2);
            } else if (v.isArray()) {
                yaml += space + key + ":\n" + jsonToYaml(v, indent + 2);
            } else {
                QString valStr = v.toVariant().toString();
                // Simple quoting logic
                bool needsQuotes = valStr.isEmpty() || valStr.contains(':') || valStr.contains('#') || 
                                   valStr.contains('[') || valStr.contains(']') || valStr.contains('{') || 
                                   valStr.contains('}') || valStr.contains(',') || valStr.contains('&') || 
                                   valStr.contains('*') || valStr.contains('!') || valStr.contains('|') || 
                                   valStr.contains('>') || valStr.contains('<') || valStr.contains('%') || 
                                   valStr.contains('@') || valStr.startsWith(' ') || valStr.endsWith(' ');
                if (needsQuotes) {
                    valStr = "\"" + valStr.replace("\"", "\\\"") + "\"";
                }
                yaml += space + key + ": " + valStr + "\n";
            }
        }
    } else if (value.isArray()) {
        QJsonArray arr = value.toArray();
        for (const QJsonValue &v : arr) {
            if (v.isObject()) {
                // For objects in an array, the first line of the object gets the "- "
                QString nested = jsonToYaml(v, indent + 2);
                if (!nested.isEmpty()) {
                    QStringList lines = nested.split('\n', Qt::SkipEmptyParts);
                    if (!lines.isEmpty()) {
                        yaml += space + "- " + lines[0].trimmed() + "\n";
                        for (int i = 1; i < lines.size(); ++i) {
                            yaml += lines[i] + "\n";
                        }
                    }
                }
            } else if (v.isArray()) {
                 yaml += space + "- \n" + jsonToYaml(v, indent + 2);
            } else {
                yaml += space + "- " + v.toVariant().toString() + "\n";
            }
        }
    }
    return yaml;
}

void ProfilesView::onEditProfile()
{
    QModelIndexList selected = ui->tableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) return;
    
    QModelIndex sourceIndex = m_proxyModel->mapToSource(selected.first());
    QJsonObject dto = m_model->getProfile(sourceIndex.row());
    
    // EdgeX V3 Profile JSON is { apiVersion: "v3", profile: { ... } }
    QJsonObject profile = dto["profile"].toObject();
    if (profile.isEmpty()) profile = dto; 

    // Include apiVersion at the top level
    profile["apiVersion"] = dto["apiVersion"].toString("v3");

    // Clean up recursive IDs and timestamps
    auto cleanRecursive = [](QJsonObject &obj, auto& self) -> void {
        obj.remove("id");
        obj.remove("created");
        obj.remove("modified");
        obj.remove("origin");
        obj.remove("operatingState");
        for (const QString &key : obj.keys()) {
            if (obj[key].isObject()) {
                QJsonObject sub = obj[key].toObject();
                self(sub, self);
                obj[key] = sub;
            } else if (obj[key].isArray()) {
                QJsonArray arr = obj[key].toArray();
                for (int i=0; i<arr.size(); ++i) {
                    if (arr[i].isObject()) {
                        QJsonObject sub = arr[i].toObject();
                        self(sub, self);
                        arr[i] = sub;
                    }
                }
                obj[key] = arr;
            }
        }
    };
    cleanRecursive(profile, cleanRecursive);

    // Convert to YAML
    QString content = jsonToYaml(profile);

    AddProfileDialog dlg(this);
    dlg.setYaml(content);
    dlg.setActionButtonText("Save Changes");
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
