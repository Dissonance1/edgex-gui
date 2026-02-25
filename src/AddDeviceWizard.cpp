#include "AddDeviceWizard.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QMessageBox>
#include <QDebug>
#include "ConfigManager.h"

// Helper to access protected registerField
class AccessibleWizardPage : public QWizardPage {
public:
    using QWizardPage::registerField;
};

enum { Page_Service, Page_Profile, Page_Info, Page_Protocol };

AddDeviceWizard::AddDeviceWizard(QWidget *parent, const QJsonObject &editData)
    : QWizard(parent), m_editData(editData), m_client(new MetadataClient(this))
{
    m_isEdit = !m_editData.isEmpty();
    setWindowTitle(m_isEdit ? "Edit Device" : "Add Device Wizard");
    // setWizardStyle(ModernStyle);
    
    // Create Pages
    setPage(Page_Service, createServicePage());
    setPage(Page_Profile, createProfilePage());
    setPage(Page_Info, createInfoPage());
    setPage(Page_Protocol, createProtocolPage());

    // Connect Client Signals
    connect(m_client, &MetadataClient::deviceServicesReceived, this, &AddDeviceWizard::onServicesReceived);
    connect(m_client, &MetadataClient::deviceProfilesReceived, this, &AddDeviceWizard::onProfilesReceived);

    // Initial Data Fetch
    m_client->setBaseUrl(ConfigManager::instance().metadataUrl());
    qDebug() << "AddDeviceWizard: Fetching metadata from" << ConfigManager::instance().metadataUrl();
    m_client->fetchDeviceServices();
    m_client->fetchDeviceProfiles();
}

QWizardPage *AddDeviceWizard::createServicePage() {
    // QWizardPage *page = new QWizardPage();
    AccessibleWizardPage *page = new AccessibleWizardPage();
    page->setTitle("Select Device Service");
    QVBoxLayout *layout = new QVBoxLayout(page);
    m_serviceCombo = new QComboBox();
    layout->addWidget(new QLabel("Select the service that will manage this device:"));
    layout->addWidget(m_serviceCombo);
    
    // Validation: Require a selection
    page->registerField("serviceName", m_serviceCombo); 
    return page;
}

QWizardPage *AddDeviceWizard::createProfilePage() {
    // QWizardPage *page = new QWizardPage();
    AccessibleWizardPage *page = new AccessibleWizardPage();
    page->setTitle("Select Device Profile");
    QVBoxLayout *layout = new QVBoxLayout(page);
    m_profileCombo = new QComboBox();
    layout->addWidget(new QLabel("Select the profile for this device:"));
    layout->addWidget(m_profileCombo);
    
    page->registerField("profileName", m_profileCombo);
    return page;
}

QWizardPage *AddDeviceWizard::createInfoPage() {
    // QWizardPage *page = new QWizardPage();
    AccessibleWizardPage *page = new AccessibleWizardPage();
    page->setTitle("Device Primary Info");
    QFormLayout *layout = new QFormLayout(page);
    
    m_nameEdit = new QLineEdit();
    layout->addRow("Name (Required):", m_nameEdit);
    // page->registerField("deviceName*", m_nameEdit);
    page->registerField("deviceName", m_nameEdit);

    m_descEdit = new QLineEdit();
    layout->addRow("Description:", m_descEdit);
    
    m_labelsEdit = new QLineEdit();
    m_labelsEdit->setPlaceholderText("label1, label2");
    layout->addRow("Labels:", m_labelsEdit);

    m_adminStateCombo = new QComboBox();
    m_adminStateCombo->addItems({"UNLOCKED", "LOCKED"});
    layout->addRow("Admin State:", m_adminStateCombo);

    if (m_isEdit) {
        QJsonObject dObj = m_editData;
        if (m_editData.contains("device")) dObj = m_editData["device"].toObject();

        m_nameEdit->setText(dObj["name"].toString());
        m_nameEdit->setReadOnly(true); // Renaming not supported by EdgeX v3 PATCH
        m_descEdit->setText(dObj["description"].toString());
        
        QJsonArray labels = dObj["labels"].toArray();
        QStringList labelsList;
        for (const auto &v : labels) labelsList << v.toString();
        m_labelsEdit->setText(labelsList.join(", "));
        
        m_adminStateCombo->setCurrentText(dObj["adminState"].toString());
    }
    
    return page;
}

QWizardPage *AddDeviceWizard::createProtocolPage() {
    // QWizardPage *page = new QWizardPage();
    AccessibleWizardPage *page = new AccessibleWizardPage();
    page->setTitle("Protocols Configuration");
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->addWidget(new QLabel("Define protocols in JSON format:"));
    
    m_protocolEdit = new QTextEdit();
    if (m_isEdit) {
        QJsonObject dObj = m_editData;
        if (m_editData.contains("device")) dObj = m_editData["device"].toObject();
        m_protocolEdit->setPlainText(QJsonDocument(dObj["protocols"].toObject()).toJson());
    } else {
        m_protocolEdit->setPlainText("{\n  \"other\": {\n    \"Address\": \"localhost\",\n    \"Port\": \"1234\"\n  }\n}");
    }
    layout->addWidget(m_protocolEdit);
    return page;
}

bool AddDeviceWizard::validateCurrentPage() {
    int id = currentId();
    qDebug() << "AddDeviceWizard: Validating/Leaving page" << id;
    
    if (id == Page_Protocol) { 
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(m_protocolEdit->toPlainText().toUtf8(), &error);
        
        if (error.error != QJsonParseError::NoError) {
            qDebug() << "AddDeviceWizard: Protocol JSON error:" << error.errorString();
            QMessageBox::warning(this, "JSON Error", "The protocol configuration is not valid JSON:\n" + error.errorString());
            return false;
        }
    }
    return true;
}

int AddDeviceWizard::nextId() const {
    int id = currentId();
    if (id == Page_Protocol) return -1;
    return id + 1;
}

void AddDeviceWizard::onServicesReceived(const QJsonArray &services) {
    qDebug() << "AddDeviceWizard: Received" << services.size() << "services";
    m_serviceCombo->clear();
    for (const auto &v : services) {
        QString name = v.toObject()["name"].toString();
        m_serviceCombo->addItem(name);
    }
    if (m_isEdit) {
        QJsonObject dObj = m_editData;
        if (m_editData.contains("device")) dObj = m_editData["device"].toObject();
        m_serviceCombo->setCurrentText(dObj["serviceName"].toString());
    } else if (m_serviceCombo->count() > 0) {
        m_serviceCombo->setCurrentIndex(0);
    }
}

void AddDeviceWizard::onProfilesReceived(const QJsonArray &profiles) {
    qDebug() << "AddDeviceWizard: Received" << profiles.size() << "profiles";
    m_profileCombo->clear();
    for (const auto &v : profiles) {
        QString name = v.toObject()["name"].toString();
        m_profileCombo->addItem(name);
    }
    if (m_isEdit) {
        QJsonObject dObj = m_editData;
        if (m_editData.contains("device")) dObj = m_editData["device"].toObject();
        m_profileCombo->setCurrentText(dObj["profileName"].toString());
    } else if (m_profileCombo->count() > 0) {
        m_profileCombo->setCurrentIndex(0);
    }
}

QJsonObject AddDeviceWizard::deviceData() const {
    QJsonObject device;
    device["apiVersion"] = "v3";
    
    QJsonObject dObj;
    dObj["name"] = m_nameEdit->text();
    dObj["description"] = m_descEdit->text();
    
    QJsonArray labels;
    QStringList labelsList = m_labelsEdit->text().split(',', Qt::SkipEmptyParts);
    for (const QString& label : labelsList) {
        labels.append(label.trimmed());
    }
    dObj["labels"] = labels;
    
    dObj["serviceName"] = m_serviceCombo->currentText();
    dObj["profileName"] = m_profileCombo->currentText();
    dObj["adminState"] = m_adminStateCombo->currentText();
    
    if (m_isEdit) {
        // Renaming is not supported by standard EdgeX v3 PATCH. 
        // We use the name to identify the device in the PATCH request.
    }

    // Protocol Parsing (guaranteed valid by validateCurrentPage)
    dObj["protocols"] = QJsonDocument::fromJson(m_protocolEdit->toPlainText().toUtf8()).object();
    
    device["device"] = dObj;
    return device;
}