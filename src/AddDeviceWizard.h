#ifndef ADDDEVICEWIZARD_H
#define ADDDEVICEWIZARD_H

#include <QWizard>
#include <QJsonArray>
#include <QJsonObject>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include "MetadataClient.h"

class AddDeviceWizard : public QWizard
{
    Q_OBJECT
public:
    explicit AddDeviceWizard(QWidget *parent = nullptr, const QJsonObject &editData = QJsonObject());

    QJsonObject deviceData() const;

private slots:
    void onServicesReceived(const QJsonArray &services);
    void onProfilesReceived(const QJsonArray &profiles);

private:
    QWizardPage *createServicePage();
    QWizardPage *createProfilePage();
    QWizardPage *createInfoPage();
    QWizardPage *createProtocolPage();

    QJsonObject m_editData;
    bool m_isEdit;
    MetadataClient *m_client;
    
    QComboBox *m_serviceCombo;
    QComboBox *m_profileCombo;
    QLineEdit *m_nameEdit;
    QLineEdit *m_descEdit;
    QLineEdit *m_labelsEdit;
    QComboBox *m_adminStateCombo;
    QTextEdit *m_protocolEdit;
};

#endif // ADDDEVICEWIZARD_H
