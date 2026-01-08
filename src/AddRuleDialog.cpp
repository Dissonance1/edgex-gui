#include "AddRuleDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>

AddRuleDialog::AddRuleDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

void AddRuleDialog::setupUi()
{
    setWindowTitle("Add eKuiper Rule (Form Based)");
    resize(700, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QTabWidget *tabs = new QTabWidget();
    tabs->addTab(createGeneralTab(), "1. Definition");
    tabs->addTab(createSinksTab(), "2. Sinks (Actions)");
    tabs->addTab(createOptionsTab(), "3. Options");
    mainLayout->addWidget(tabs);

    QHBoxLayout *buttons = new QHBoxLayout();
    QPushButton *btnOk = new QPushButton("Create Rule");
    QPushButton *btnCancel = new QPushButton("Cancel");
    buttons->addStretch();
    buttons->addWidget(btnOk);
    buttons->addWidget(btnCancel);
    mainLayout->addLayout(buttons);

    connect(btnOk, &QPushButton::clicked, [this]() {
        if (m_idEdit->text().isEmpty() || m_sqlEdit->toPlainText().isEmpty()) {
            QMessageBox::warning(this, "Validation", "Rule Name and SQL are required.");
            return;
        }
        accept();
    });
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

QWidget* AddRuleDialog::createGeneralTab()
{
    QWidget *w = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(w);

    QFormLayout *form = new QFormLayout();
    m_idEdit = new QLineEdit();
    m_idEdit->setPlaceholderText("e.g. alert-high-temp");
    form->addRow("Rule Name*:", m_idEdit);
    layout->addLayout(form);

    layout->addWidget(new QLabel("<b>SQL Query*:</b>"));
    m_sqlEdit = new QTextEdit();
    m_sqlEdit->setPlaceholderText("SELECT * FROM edgex_stream WHERE humidity > 80");
    m_sqlEdit->setStyleSheet("font-family: 'Consolas', monospace; background-color: #0f172a; color: #4ade80;");
    layout->addWidget(m_sqlEdit);

    return w;
}

QWidget* AddRuleDialog::createSinksTab()
{
    QWidget *w = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(w);

    QHBoxLayout *header = new QHBoxLayout();
    header->addWidget(new QLabel("Sink Type:"));
    m_sinkTypeCombo = new QComboBox();
    m_sinkTypeCombo->addItems({"Log", "EdgeX Message Bus", "MQTT", "REST"});
    header->addWidget(m_sinkTypeCombo);
    header->addStretch();
    layout->addLayout(header);

    m_sinkStack = new QStackedWidget();
    
    // 0: Log
    QWidget *logW = new QWidget();
    QVBoxLayout *logL = new QVBoxLayout(logW);
    logL->addWidget(new QLabel("The results will be printed to the eKuiper logs."));
    logL->addStretch();
    m_sinkStack->addWidget(logW);

    // 1: EdgeX
    QWidget *edgexW = new QWidget();
    QFormLayout *edgexF = new QFormLayout(edgexW);
    m_edgexProtocolCombo = new QComboBox();
    m_edgexProtocolCombo->addItems({"redis", "tcp"});
    edgexF->addRow("Protocol:", m_edgexProtocolCombo);
    m_edgexHostEdit = new QLineEdit("edgex-redis");
    edgexF->addRow("Host:", m_edgexHostEdit);
    m_edgexPortEdit = new QLineEdit("6379");
    edgexF->addRow("Port:", m_edgexPortEdit);
    m_edgexTopicEdit = new QLineEdit("rules-events");
    edgexF->addRow("Topic:", m_edgexTopicEdit);
    m_edgexMsgTypeCombo = new QComboBox();
    m_edgexMsgTypeCombo->addItems({"event", "request"});
    edgexF->addRow("Message Type:", m_edgexMsgTypeCombo);
    m_sinkStack->addWidget(edgexW);

    // 2: MQTT
    QWidget *mqttW = new QWidget();
    QFormLayout *mqttF = new QFormLayout(mqttW);
    m_mqttBrokerEdit = new QLineEdit("tcp://broker.emqx.io:1883");
    mqttF->addRow("Broker URL:", m_mqttBrokerEdit);
    m_mqttTopicEdit = new QLineEdit("edgex/results");
    mqttF->addRow("Topic:", m_mqttTopicEdit);
    m_mqttClientIdEdit = new QLineEdit("ekuiper_qt_client");
    mqttF->addRow("Client ID:", m_mqttClientIdEdit);
    m_mqttQosSpin = new QSpinBox();
    m_mqttQosSpin->setRange(0, 2);
    mqttF->addRow("QoS:", m_mqttQosSpin);
    m_sinkStack->addWidget(mqttW);

    // 3: REST
    QWidget *restW = new QWidget();
    QFormLayout *restF = new QFormLayout(restW);
    m_restUrlEdit = new QLineEdit("http://localhost:8080/api/v1/result");
    restF->addRow("Endpoint URL:", m_restUrlEdit);
    m_restMethodCombo = new QComboBox();
    m_restMethodCombo->addItems({"POST", "PUT", "GET"});
    restF->addRow("HTTP Method:", m_restMethodCombo);
    m_sinkStack->addWidget(restW);

    layout->addWidget(m_sinkStack);
    connect(m_sinkTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), m_sinkStack, &QStackedWidget::setCurrentIndex);

    return w;
}

QWidget* AddRuleDialog::createOptionsTab()
{
    QWidget *w = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(w);
    QFormLayout *form = new QFormLayout();

    m_debugCheck = new QCheckBox("Enable Debug Mode");
    form->addRow("", m_debugCheck);

    m_logPlanCheck = new QCheckBox("Log Execution Plan");
    form->addRow("", m_logPlanCheck);

    m_sendMetaCheck = new QCheckBox("Send Metadata to Sink");
    form->addRow("", m_sendMetaCheck);

    m_qosSpin = new QSpinBox();
    m_qosSpin->setRange(0, 2);
    form->addRow("Global QoS:", m_qosSpin);

    m_checkpointSpin = new QSpinBox();
    m_checkpointSpin->setRange(0, 3600);
    m_checkpointSpin->setSuffix(" seconds");
    m_checkpointSpin->setValue(300);
    form->addRow("Checkpoint Interval:", m_checkpointSpin);

    layout->addLayout(form);
    layout->addStretch();
    return w;
}

QJsonObject AddRuleDialog::ruleData() const
{
    QJsonObject root;
    root["id"] = m_idEdit->text();
    root["sql"] = m_sqlEdit->toPlainText();

    // Actions (Sinks)
    QJsonArray actions;
    QJsonObject sink;
    int sinkType = m_sinkTypeCombo->currentIndex();
    if (sinkType == 0) { // Log
        sink["log"] = QJsonObject();
    } else if (sinkType == 1) { // EdgeX
        QJsonObject edgex;
        edgex["protocol"] = m_edgexProtocolCombo->currentText();
        edgex["host"] = m_edgexHostEdit->text();
        edgex["port"] = m_edgexPortEdit->text().toInt();
        edgex["topic"] = m_edgexTopicEdit->text();
        edgex["messageType"] = m_edgexMsgTypeCombo->currentText();
        edgex["contentType"] = "application/json";
        sink["edgex"] = edgex;
    } else if (sinkType == 2) { // MQTT
        QJsonObject mqtt;
        mqtt["server"] = m_mqttBrokerEdit->text();
        mqtt["topic"] = m_mqttTopicEdit->text();
        mqtt["clientId"] = m_mqttClientIdEdit->text();
        mqtt["qos"] = m_mqttQosSpin->value();
        sink["mqtt"] = mqtt;
    } else if (sinkType == 3) { // REST
        QJsonObject rest;
        rest["url"] = m_restUrlEdit->text();
        rest["method"] = m_restMethodCombo->currentText();
        sink["rest"] = rest;
    }
    actions.append(sink);
    root["actions"] = actions;

    // Options
    QJsonObject opts;
    opts["debug"] = m_debugCheck->isChecked();
    opts["logPlan"] = m_logPlanCheck->isChecked();
    opts["sendMetaToSink"] = m_sendMetaCheck->isChecked();
    opts["qos"] = m_qosSpin->value();
    opts["checkpointInterval"] = m_checkpointSpin->value();
    root["options"] = opts;

    return root;
}

QString AddRuleDialog::id() const { return m_idEdit->text(); }

void AddRuleDialog::onSinkTypeChanged(int index) { m_sinkStack->setCurrentIndex(index); }
