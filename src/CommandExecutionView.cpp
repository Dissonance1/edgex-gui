#include "CommandExecutionView.h"
#include "ConfigManager.h"
#include <QGroupBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QJsonDocument>

CommandExecutionView::CommandExecutionView(const QString &deviceName, QWidget *parent)
    : QWidget(parent), m_deviceName(deviceName), m_client(new CommandClient(this))
{
    setupUi();
    connect(m_client, &CommandClient::commandsReceived, this, &CommandExecutionView::onCommandsReceived);
    connect(m_client, &CommandClient::commandExecuted, this, &CommandExecutionView::onCommandExecuted);

    m_client->setBaseUrl(ConfigManager::instance().metadataUrl().replace("59881", "59882")); // Command service default
    m_client->fetchDeviceCommands(m_deviceName);
}

void CommandExecutionView::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QLabel *header = new QLabel("Commands for: " + m_deviceName);
    header->setStyleSheet("font-size: 18px; font-weight: bold; margin-bottom: 10px;");
    mainLayout->addWidget(header);

    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    QWidget *content = new QWidget();
    m_scrollLayout = new QVBoxLayout(content);
    m_scrollLayout->addStretch();
    scroll->setWidget(content);
    
    mainLayout->addWidget(scroll);
}

void CommandExecutionView::onCommandsReceived(const QJsonArray &commands)
{
    // Clear stretch
    QLayoutItem *item;
    while ((item = m_scrollLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    for (const auto &v : commands) {
        createCommandWidget(v.toObject());
    }
    m_scrollLayout->addStretch();
}

void CommandExecutionView::createCommandWidget(const QJsonObject &command)
{
    QString name = command["name"].toString();
    QGroupBox *group = new QGroupBox(name);
    QVBoxLayout *layout = new QVBoxLayout(group);

    if (command["get"].toBool()) {
        QPushButton *btnGet = new QPushButton("Execute GET");
        btnGet->setStyleSheet("background-color: #2ecc71; color: white;");
        connect(btnGet, &QPushButton::clicked, this, [this, name]() { executeGet(name); });
        layout->addWidget(btnGet);
    }

    if (command["set"].toBool()) {
        QJsonArray params = command["parameters"].toArray();
        QFormLayout *form = new QFormLayout();
        for (const auto &p : params) {
            form->addRow(p.toObject()["resourceName"].toString() + ":", createParameterWidget(p.toObject(), name));
        }
        layout->addLayout(form);

        QPushButton *btnPut = new QPushButton("Execute PUT");
        btnPut->setStyleSheet("background-color: #e67e22; color: white;");
        connect(btnPut, &QPushButton::clicked, this, [this, name, params]() { executePut(name, params); });
        layout->addWidget(btnPut);
    }

    m_scrollLayout->insertWidget(m_scrollLayout->count() - 1, group);
}

QWidget* CommandExecutionView::createParameterWidget(const QJsonObject &param, const QString &cmdName)
{
    QString resName = param["resourceName"].toString();
    QString key = cmdName + "_" + resName;
    QLineEdit *edit = new QLineEdit();
    edit->setPlaceholderText(param["valueType"].toString());
    m_paramInputs[key] = edit;
    return edit;
}

void CommandExecutionView::executeGet(const QString &commandName)
{
    m_client->executeCommand(m_deviceName, commandName, "GET");
}

void CommandExecutionView::executePut(const QString &commandName, const QJsonArray &parameters)
{
    QJsonObject params;
    for (const auto &p : parameters) {
        QString resName = p.toObject()["resourceName"].toString();
        QString key = commandName + "_" + resName;
        QLineEdit *edit = m_paramInputs[key];
        if (edit) {
            params[resName] = edit->text();
        }
    }
    m_client->executeCommand(m_deviceName, commandName, "PUT", params);
}

void CommandExecutionView::onCommandExecuted(bool success, const QString &message)
{
    if (success) {
        QMessageBox::information(this, "Success", "Command executed successfully.");
    } else {
        QMessageBox::critical(this, "Error", "Command execution failed: " + message);
    }
}
