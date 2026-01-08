#include "SettingsDialog.h"
#include "ConfigManager.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QHBoxLayout>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    m_metadataEdit->setText(ConfigManager::instance().metadataUrl());
    m_dataEdit->setText(ConfigManager::instance().coreDataUrl());
    m_commandEdit->setText(ConfigManager::instance().commandUrl());
    m_systemEdit->setText(ConfigManager::instance().systemUrl());
    m_notificationsEdit->setText(ConfigManager::instance().notificationsUrl());
    m_schedulerEdit->setText(ConfigManager::instance().schedulerUrl());
    m_rulesEdit->setText(ConfigManager::instance().rulesUrl());
}

void SettingsDialog::setupUi()
{
    setWindowTitle("EdgeX Settings");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QFormLayout *form = new QFormLayout();
    m_metadataEdit = new QLineEdit();
    m_dataEdit = new QLineEdit();
    m_commandEdit = new QLineEdit();
    m_systemEdit = new QLineEdit();
    m_notificationsEdit = new QLineEdit();
    m_schedulerEdit = new QLineEdit();
    m_rulesEdit = new QLineEdit();
 
    form->addRow("Metadata URL:", m_metadataEdit);
    form->addRow("Core Data URL:", m_dataEdit);
    form->addRow("Command URL:", m_commandEdit);
    form->addRow("System URL:", m_systemEdit);
    form->addRow("Notifications URL:", m_notificationsEdit);
    form->addRow("Scheduler URL:", m_schedulerEdit);
    form->addRow("Rules Engine URL:", m_rulesEdit);
    mainLayout->addLayout(form);

    QHBoxLayout *buttons = new QHBoxLayout();
    QPushButton *btnSave = new QPushButton("Save");
    QPushButton *btnCancel = new QPushButton("Cancel");
    buttons->addStretch();
    buttons->addWidget(btnSave);
    buttons->addWidget(btnCancel);
    mainLayout->addLayout(buttons);

    connect(btnSave, &QPushButton::clicked, this, &SettingsDialog::onSave);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

void SettingsDialog::onSave()
{
    ConfigManager::instance().setMetadataUrl(m_metadataEdit->text());
    ConfigManager::instance().setCoreDataUrl(m_dataEdit->text());
    ConfigManager::instance().setCommandUrl(m_commandEdit->text());
    ConfigManager::instance().setSystemUrl(m_systemEdit->text());
    ConfigManager::instance().setNotificationsUrl(m_notificationsEdit->text());
    ConfigManager::instance().setSchedulerUrl(m_schedulerEdit->text());
    ConfigManager::instance().setRulesUrl(m_rulesEdit->text());
    ConfigManager::instance().save();
    accept();
}
