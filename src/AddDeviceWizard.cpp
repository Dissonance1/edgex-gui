#include "AddDeviceWizard.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include <QMessageBox>

// Helper to access protected registerField
class AccessibleWizardPage : public QWizardPage {
public:
    using QWizardPage::registerField;
};

AddDeviceWizard::AddDeviceWizard(QWidget *parent, const QJsonObject &editData)
    : QWizard(parent), m_editData(editData), m_client(new MetadataClient(this))
{
    m_isEdit = !m_editData.isEmpty();
    setWindowTitle(m_isEdit ? "Edit Device" : "Add Device Wizard");
    setWizardStyle(ModernStyle);
    
    // Create Pages
    addPage(createServicePage());
    addPage(createProfilePage());
    addPage(createInfoPage());
    addPage(createProtocolPage());

    // Connect Client Signals
    connect(m_client, &MetadataClient::deviceServicesReceived, this, &AddDeviceWizard::onServicesReceived);
    connect(m_client, &MetadataClient::deviceProfilesReceived, this, &AddDeviceWizard::onProfilesReceived);

    // Initial Data Fetch
    m_client->setBaseUrl("http://localhost:59881"); // Example EdgeX Metadata URL
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
    
    // Validation: Require a selection (index != -1)
    page->registerField("serviceName*", m_serviceCombo); 
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
    
    page->registerField("profileName*", m_profileCombo);
    return page;
}

QWizardPage *AddDeviceWizard::createInfoPage() {
    // QWizardPage *page = new QWizardPage();
    AccessibleWizardPage *page = new AccessibleWizardPage();
    page->setTitle("Device Primary Info");
    QFormLayout *layout = new QFormLayout(page);
    
    m_nameEdit = new QLineEdit();
    layout->addRow("Name (Required):", m_nameEdit);
    // MANDATORY FIELD: Wizard disables 'Next' if empty
    page->registerField("deviceName*", m_nameEdit);

    if (m_isEdit) {
        m_nameEdit->setText(m_editData["name"].toString());
        m_nameEdit->setReadOnly(true);
    }
    
    m_descEdit = new QLineEdit();
    layout->addRow("Description:", m_descEdit);
    
    m_labelsEdit = new QLineEdit();
    m_labelsEdit->setPlaceholderText("label1, label2");
    layout->addRow("Labels:", m_labelsEdit);
    
    m_adminStateCombo = new QComboBox();
    m_adminStateCombo->addItems({"UNLOCKED", "LOCKED"});
    layout->addRow("Admin State:", m_adminStateCombo);
    
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
        m_protocolEdit->setPlainText(QJsonDocument(m_editData["protocols"].toObject()).toJson());
    } else {
        m_protocolEdit->setPlainText("{\n  \"other\": {\n    \"Address\": \"localhost\",\n    \"Port\": \"1234\"\n  }\n}");
    }
    layout->addWidget(m_protocolEdit);
    return page;
}

bool AddDeviceWizard::validateCurrentPage() {
    // Only validate when leaving the Protocol Page (the last page)
    if (currentPage() == page(3)) { 
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(m_protocolEdit->toPlainText().toUtf8(), &error);
        
        if (error.error != QJsonParseError::NoError) {
            QMessageBox::warning(this, "JSON Error", "The protocol configuration is not valid JSON:\n" + error.errorString());
            return false; // Stay on page
        }
    }
    return true; // Allow transition
}

void AddDeviceWizard::onServicesReceived(const QJsonArray &services) {
    m_serviceCombo->clear();
    for (const auto &v : services) m_serviceCombo->addItem(v.toObject()["name"].toString());
}

void AddDeviceWizard::onProfilesReceived(const QJsonArray &profiles) {
    m_profileCombo->clear();
    for (const auto &v : profiles) m_profileCombo->addItem(v.toObject()["name"].toString());
}

QJsonObject AddDeviceWizard::deviceData() const {
    QJsonObject device;
    device["apiVersion"] = "v3";
    
    QJsonObject dObj;
    dObj["name"] = m_nameEdit->text();
    dObj["description"] = m_descEdit->text();
    dObj["serviceName"] = m_serviceCombo->currentText();
    dObj["profileName"] = m_profileCombo->currentText();
    dObj["adminState"] = m_adminStateCombo->currentText();
    
    // Protocol Parsing (guaranteed valid by validateCurrentPage)
    dObj["protocols"] = QJsonDocument::fromJson(m_protocolEdit->toPlainText().toUtf8()).object();
    
    device["device"] = dObj;
    return device;
}