#ifndef ADDDEVICEWIZARD_H
#define ADDDEVICEWIZARD_H

#include <QWizard>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

/*
// Simple Metadata Client for EdgeX API
class MetadataClient : public QObject {
    Q_OBJECT
public:
    explicit MetadataClient(QObject *parent = nullptr) : QObject(parent) {}
    void setBaseUrl(const QString &url) { baseUrl = url; }

    void fetchDeviceServices() {
        // Implementation would use QNetworkAccessManager to GET /api/v3/deviceservice/all
        // For brevity, assume it emits deviceServicesReceived(array)
    }

    void fetchDeviceProfiles() {
        // Implementation would use QNetworkAccessManager to GET /api/v3/deviceprofile/all
    }

signals:
    void deviceServicesReceived(const QJsonArray &services);
    void deviceProfilesReceived(const QJsonArray &profiles);

private:
    QString baseUrl;
};
*/
#include "MetadataClient.h"

class AddDeviceWizard : public QWizard
{
    Q_OBJECT

public:
    AddDeviceWizard(QWidget *parent = nullptr, const QJsonObject &editData = QJsonObject());
    QJsonObject deviceData() const;

    // Overriding validateCurrentPage for JSON validation
    bool validateCurrentPage() override;

private:
    QWizardPage *createServicePage();
    QWizardPage *createProfilePage();
    QWizardPage *createInfoPage();
    QWizardPage *createProtocolPage();

    void onServicesReceived(const QJsonArray &services);
    void onProfilesReceived(const QJsonArray &profiles);

    MetadataClient *m_client;
    QJsonObject m_editData;
    bool m_isEdit;

    // UI Elements
    QComboBox *m_serviceCombo;
    QComboBox *m_profileCombo;
    QLineEdit *m_nameEdit;
    QLineEdit *m_descEdit;
    QLineEdit *m_labelsEdit;
    QComboBox *m_adminStateCombo;
    QTextEdit *m_protocolEdit;
};

#endif