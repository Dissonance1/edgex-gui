#include "AddDeviceWizard.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFormLayout>
#include "ConfigManager.h"

AddDeviceWizard::AddDeviceWizard(QWidget *parent, const QJsonObject &editData)
    : QWizard(parent), m_editData(editData), m_client(new MetadataClient(this))
{
    m_isEdit = !m_editData.isEmpty();
    setWindowTitle(m_isEdit ? "Edit Device" : "Add Device Wizard");
    setWizardStyle(ModernStyle);
    
    addPage(createServicePage());
    addPage(createProfilePage());
    addPage(createInfoPage());
    addPage(createProtocolPage());

    connect(m_client, &MetadataClient::deviceServicesReceived, this, &AddDeviceWizard::onServicesReceived);
    connect(m_client, &MetadataClient::deviceProfilesReceived, this, &AddDeviceWizard::onProfilesReceived);

    m_client->setBaseUrl(ConfigManager::instance().metadataUrl());
    m_client->fetchDeviceServices();
    m_client->fetchDeviceProfiles();
}

QWizardPage *AddDeviceWizard::createServicePage()
{
    QWizardPage *page = new QWizardPage();
    page->setTitle("Select Device Service");
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->addWidget(new QLabel("Select the service that will manage this device:"));
    
    m_serviceCombo = new QComboBox();
    layout->addWidget(m_serviceCombo);
    layout->addStretch();
    
    return page;
}

QWizardPage *AddDeviceWizard::createProfilePage()
{
    QWizardPage *page = new QWizardPage();
    page->setTitle("Select Device Profile");
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->addWidget(new QLabel("Select the profile for this device:"));
    
    m_profileCombo = new QComboBox();
    layout->addWidget(m_profileCombo);
    layout->addStretch();
    
    return page;
}

QWizardPage *AddDeviceWizard::createInfoPage()
{
    QWizardPage *page = new QWizardPage();
    page->setTitle("Device Primary Info");
    QFormLayout *layout = new QFormLayout(page);
    
    m_nameEdit = new QLineEdit();
    layout->addRow("Name:", m_nameEdit);
    if (m_isEdit) {
        m_nameEdit->setText(m_editData["name"].toString());
        m_nameEdit->setReadOnly(true); // Name usually cannot be changed in EdgeX
    }
    
    m_descEdit = new QLineEdit();
    layout->addRow("Description:", m_descEdit);
    if (m_isEdit) m_descEdit->setText(m_editData["description"].toString());
    
    m_labelsEdit = new QLineEdit();
    m_labelsEdit->setPlaceholderText("label1, label2");
    layout->addRow("Labels:", m_labelsEdit);
    if (m_isEdit) {
        QJsonArray labels = m_editData["labels"].toArray();
        QStringList joined;
        for (const auto &l : labels) joined << l.toString();
        m_labelsEdit->setText(joined.join(", "));
    }
    
    m_adminStateCombo = new QComboBox();
    m_adminStateCombo->addItems({"UNLOCKED", "LOCKED"});
    layout->addRow("Admin State:", m_adminStateCombo);
    if (m_isEdit) m_adminStateCombo->setCurrentText(m_editData["adminState"].toString());
    
    return page;
}

QWizardPage *AddDeviceWizard::createProtocolPage()
{
    QWizardPage *page = new QWizardPage();
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

void AddDeviceWizard::onServicesReceived(const QJsonArray &services)
{
    m_serviceCombo->clear();
    for (const auto &v : services) {
        m_serviceCombo->addItem(v.toObject()["name"].toString());
    }
    if (m_isEdit) {
        m_serviceCombo->setCurrentText(m_editData["serviceName"].toString());
    }
}

void AddDeviceWizard::onProfilesReceived(const QJsonArray &profiles)
{
    m_profileCombo->clear();
    for (const auto &v : profiles) {
        m_profileCombo->addItem(v.toObject()["name"].toString());
    }
    if (m_isEdit) {
        m_profileCombo->setCurrentText(m_editData["profileName"].toString());
    }
}

QJsonObject AddDeviceWizard::deviceData() const
{
    QJsonObject device;
    device["apiVersion"] = "v3";
    
    QJsonObject deviceObj;
    deviceObj["name"] = m_nameEdit->text();
    deviceObj["description"] = m_descEdit->text();
    deviceObj["adminState"] = m_adminStateCombo->currentText();
    deviceObj["operatingState"] = "UP";
    deviceObj["serviceName"] = m_serviceCombo->currentText();
    deviceObj["profileName"] = m_profileCombo->currentText();
    
    QStringList labels = m_labelsEdit->text().split(',', Qt::SkipEmptyParts);
    QJsonArray labelsArr;
    for (const auto &l : labels) labelsArr.append(l.trimmed());
    deviceObj["labels"] = labelsArr;
    
    // Parse protocol JSON
    QJsonDocument doc = QJsonDocument::fromJson(m_protocolEdit->toPlainText().toUtf8());
    if (doc.isObject()) {
        deviceObj["protocols"] = doc.object();
    }
    
    device["device"] = deviceObj;
    return device;
}
