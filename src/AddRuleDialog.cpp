#include "AddRuleDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QScrollArea>
#include <QGroupBox>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QStackedWidget>
#include <QTabWidget>

AddRuleDialog::AddRuleDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

void AddRuleDialog::setupUi()
{
    setWindowTitle("Add eKuiper Rule (Enhanced)");
    resize(850, 750);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QTabWidget *tabs = new QTabWidget();
    tabs->addTab(createGeneralTab(), "1. Definition");
    tabs->addTab(createSinksTab(), "2. Sinks (Actions)");
    tabs->addTab(createRuleOptionsTab(), "3. Rule Options");
    tabs->addTab(createSinkOptionsTab(), "4. Sink Options");
    mainLayout->addWidget(tabs);

    QHBoxLayout *buttons = new QHBoxLayout();
    m_btnOk = new QPushButton("Create Rule");
    QPushButton *btnCancel = new QPushButton("Cancel");
    buttons->addStretch();
    buttons->addWidget(m_btnOk);
    buttons->addWidget(btnCancel);
    mainLayout->addLayout(buttons);

    connect(m_btnOk, &QPushButton::clicked, [this]() {
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
    m_sqlEdit->setPlaceholderText("SELECT * FROM EdgexStream WHERE confidence > 0.6");
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
    m_mqttQosSpin = new QSpinBox(); m_mqttQosSpin->setRange(0, 2);
    mqttF->addRow("QoS:", m_mqttQosSpin);
    m_mqttUserEdit = new QLineEdit();
    mqttF->addRow("Username:", m_mqttUserEdit);
    m_mqttPassEdit = new QLineEdit(); m_mqttPassEdit->setEchoMode(QLineEdit::Password);
    mqttF->addRow("Password:", m_mqttPassEdit);
    m_mqttRetainedCheck = new QCheckBox("Retained");
    mqttF->addRow("", m_mqttRetainedCheck);
    m_sinkStack->addWidget(mqttW);

    // 3: REST
    QWidget *restW = new QWidget();
    QScrollArea *restScroll = new QScrollArea();
    restScroll->setWidgetResizable(true);
    QWidget *restContent = new QWidget();
    QFormLayout *restF = new QFormLayout(restContent);
    m_restUrlEdit = new QLineEdit("http://localhost:8080/api/v1/result");
    restF->addRow("Endpoint URL:", m_restUrlEdit);
    m_restMethodCombo = new QComboBox();
    m_restMethodCombo->addItems({"POST", "PUT", "GET", "DELETE", "PATCH"});
    restF->addRow("HTTP Method:", m_restMethodCombo);
    m_restBodyTypeCombo = new QComboBox();
    m_restBodyTypeCombo->addItems({"json", "text", "html", "xml", "form"});
    restF->addRow("Body Type:", m_restBodyTypeCombo);
    m_restTimeoutSpin = new QSpinBox(); m_restTimeoutSpin->setRange(0, 60000); m_restTimeoutSpin->setSuffix(" ms"); m_restTimeoutSpin->setValue(5000);
    restF->addRow("Timeout:", m_restTimeoutSpin);
    m_restDebugRespCheck = new QCheckBox("Debug Response");
    restF->addRow("", m_restDebugRespCheck);
    restScroll->setWidget(restContent);
    m_sinkStack->addWidget(restScroll);

    layout->addWidget(m_sinkStack);
    connect(m_sinkTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), m_sinkStack, &QStackedWidget::setCurrentIndex);

    // Common Sink Advanced Section (at bottom of sinks tab)
    QGroupBox *commonGroup = new QGroupBox("Common Properties");
    QFormLayout *commonF = new QFormLayout(commonGroup);
    m_sendSingleCheck = new QCheckBox("Send Single");
    commonF->addRow("", m_sendSingleCheck);
    m_dataTemplateEdit = new QTextEdit();
    m_dataTemplateEdit->setPlaceholderText("e.g. {{.name}} says hello");
    m_dataTemplateEdit->setFixedHeight(60);
    commonF->addRow("Data Template:", m_dataTemplateEdit);
    layout->addWidget(commonGroup);

    // Security Section (collapsible logic omitted for brevity, just a group)
    QGroupBox *secGroup = new QGroupBox("Security");
    QFormLayout *secF = new QFormLayout(secGroup);
    m_insecureSkipVerifyCheck = new QCheckBox("Insecure Skip Verify");
    secF->addRow("", m_insecureSkipVerifyCheck);
    m_certPathEdit = new QLineEdit();
    secF->addRow("Cert Path:", m_certPathEdit);
    m_privKeyPathEdit = new QLineEdit();
    secF->addRow("Priv Key Path:", m_privKeyPathEdit);
    m_rootCaPathEdit = new QLineEdit();
    secF->addRow("Root CA Path:", m_rootCaPathEdit);
    layout->addWidget(secGroup);

    return w;
}

QWidget* AddRuleDialog::createRuleOptionsTab()
{
    QWidget *w = new QWidget();
    QFormLayout *form = new QFormLayout(w);

    m_isEventTimeCheck = new QCheckBox("Is Event Time");
    form->addRow("", m_isEventTimeCheck);

    m_qosSpin = new QSpinBox(); m_qosSpin->setRange(0, 2);
    form->addRow("QoS (0: At most once, 1: At least, 2: Exactly):", m_qosSpin);

    m_lateToleranceSpin = new QSpinBox(); m_lateToleranceSpin->setRange(0, 10000); m_lateToleranceSpin->setSuffix(" ms");
    form->addRow("Late Tolerance:", m_lateToleranceSpin);

    m_concurrencySpin = new QSpinBox(); m_concurrencySpin->setRange(1, 100); m_concurrencySpin->setValue(1);
    form->addRow("Concurrency:", m_concurrencySpin);

    m_bufferLengthSpin = new QSpinBox(); m_bufferLengthSpin->setRange(0, 100000); m_bufferLengthSpin->setValue(1024);
    form->addRow("Buffer Length:", m_bufferLengthSpin);

    m_checkpointSpin = new QSpinBox(); m_checkpointSpin->setRange(0, 3600000); m_checkpointSpin->setSuffix(" ms"); m_checkpointSpin->setValue(300000);
    form->addRow("Checkpoint Interval:", m_checkpointSpin);

    m_sendMetaCheck = new QCheckBox("Send Metadata to Sink");
    form->addRow("", m_sendMetaCheck);

    m_sendErrorCheck = new QCheckBox("Send Error to Sink");
    form->addRow("", m_sendErrorCheck);

    return w;
}

QWidget* AddRuleDialog::createSinkOptionsTab()
{
    QWidget *w = new QWidget();
    QFormLayout *form = new QFormLayout(w);

    m_sinkConcurrencySpin = new QSpinBox(); m_sinkConcurrencySpin->setRange(1, 100); m_sinkConcurrencySpin->setValue(1);
    form->addRow("Sink Concurrency:", m_sinkConcurrencySpin);

    m_sinkBufferLengthSpin = new QSpinBox(); m_sinkBufferLengthSpin->setRange(0, 100000); m_sinkBufferLengthSpin->setValue(1024);
    form->addRow("Sink Buffer Length:", m_sinkBufferLengthSpin);

    m_retryIntervalSpin = new QSpinBox(); m_retryIntervalSpin->setRange(0, 60000); m_retryIntervalSpin->setSuffix(" ms"); m_retryIntervalSpin->setValue(1000);
    form->addRow("Retry Interval:", m_retryIntervalSpin);

    m_retryCountSpin = new QSpinBox(); m_retryCountSpin->setRange(0, 100); m_retryCountSpin->setValue(0);
    form->addRow("Retry Count:", m_retryCountSpin);

    m_cacheLengthSpin = new QSpinBox(); m_cacheLengthSpin->setRange(0, 1000000); m_cacheLengthSpin->setValue(1024);
    form->addRow("Cache Length:", m_cacheLengthSpin);

    m_cacheSaveIntervalSpin = new QSpinBox(); m_cacheSaveIntervalSpin->setRange(0, 3600000); m_cacheSaveIntervalSpin->setSuffix(" ms"); m_cacheSaveIntervalSpin->setValue(1000);
    form->addRow("Cache Save Interval:", m_cacheSaveIntervalSpin);

    m_runAsyncCheck = new QCheckBox("Run Asynchronously");
    form->addRow("", m_runAsyncCheck);

    m_omitIfEmptyCheck = new QCheckBox("Omit if Empty");
    form->addRow("", m_omitIfEmptyCheck);

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
    QJsonObject sinkProps;

    if (sinkType == 0) { // Log
        // log is empty or limited
    } else if (sinkType == 1) { // EdgeX
        sinkProps["protocol"] = m_edgexProtocolCombo->currentText();
        sinkProps["host"] = m_edgexHostEdit->text();
        sinkProps["port"] = m_edgexPortEdit->text().toInt();
        sinkProps["topic"] = m_edgexTopicEdit->text();
        sinkProps["messageType"] = m_edgexMsgTypeCombo->currentText();
        sinkProps["contentType"] = "application/json";
    } else if (sinkType == 2) { // MQTT
        sinkProps["server"] = m_mqttBrokerEdit->text();
        sinkProps["topic"] = m_mqttTopicEdit->text();
        sinkProps["clientId"] = m_mqttClientIdEdit->text();
        sinkProps["qos"] = m_mqttQosSpin->value();
        sinkProps["username"] = m_mqttUserEdit->text();
        sinkProps["password"] = m_mqttPassEdit->text();
        sinkProps["retained"] = m_mqttRetainedCheck->isChecked();
    } else if (sinkType == 3) { // REST
        sinkProps["url"] = m_restUrlEdit->text();
        sinkProps["method"] = m_restMethodCombo->currentText();
        sinkProps["bodyType"] = m_restBodyTypeCombo->currentText();
        sinkProps["timeout"] = m_restTimeoutSpin->value();
        sinkProps["debugResp"] = m_restDebugRespCheck->isChecked();
    }

    // Common Sink props (merged into sinkProps)
    sinkProps["sendSingle"] = m_sendSingleCheck->isChecked();
    if (!m_dataTemplateEdit->toPlainText().isEmpty()) {
        sinkProps["dataTemplate"] = m_dataTemplateEdit->toPlainText();
    }
    sinkProps["insecureSkipVerify"] = m_insecureSkipVerifyCheck->isChecked();
    if (!m_certPathEdit->text().isEmpty()) sinkProps["certificationPath"] = m_certPathEdit->text();
    if (!m_privKeyPathEdit->text().isEmpty()) sinkProps["privateKeyPath"] = m_privKeyPathEdit->text();
    if (!m_rootCaPathEdit->text().isEmpty()) sinkProps["rootCaPath"] = m_rootCaPathEdit->text();

    // Sink Advanced Options (merged into sinkProps)
    sinkProps["concurrency"] = m_sinkConcurrencySpin->value();
    sinkProps["bufferLength"] = m_sinkBufferLengthSpin->value();
    sinkProps["retryInterval"] = m_retryIntervalSpin->value();
    sinkProps["retryCount"] = m_retryCountSpin->value();
    sinkProps["cacheLength"] = m_cacheLengthSpin->value();
    sinkProps["cacheSaveInterval"] = m_cacheSaveIntervalSpin->value();
    sinkProps["runAsync"] = m_runAsyncCheck->isChecked();
    sinkProps["omitIfEmpty"] = m_omitIfEmptyCheck->isChecked();

    QString sinkName = (sinkType == 0) ? "log" : (sinkType == 1) ? "edgex" : (sinkType == 2) ? "mqtt" : "rest";
    sink[sinkName] = sinkProps;
    actions.append(sink);
    root["actions"] = actions;

    // Rule Options
    QJsonObject opts;
    opts["isEventTime"] = m_isEventTimeCheck->isChecked();
    opts["qos"] = m_qosSpin->value();
    opts["lateTolerance"] = m_lateToleranceSpin->value();
    opts["concurrency"] = m_concurrencySpin->value();
    opts["bufferLength"] = m_bufferLengthSpin->value();
    opts["checkpointInterval"] = m_checkpointSpin->value();
    opts["sendMetaToSink"] = m_sendMetaCheck->isChecked();
    opts["sendError"] = m_sendErrorCheck->isChecked();
    root["options"] = opts;

    return root;
}

QString AddRuleDialog::id() const { return m_idEdit->text(); }

void AddRuleDialog::onSinkTypeChanged(int index) { m_sinkStack->setCurrentIndex(index); }

void AddRuleDialog::setRuleData(const QString &id, const QJsonObject &data)
{
    m_idEdit->setText(id);
    m_idEdit->setReadOnly(true);
    m_sqlEdit->setPlainText(data["sql"].toString());

    // Rule Options
    QJsonObject opts = data["options"].toObject();
    m_isEventTimeCheck->setChecked(opts["isEventTime"].toBool());
    m_qosSpin->setValue(opts["qos"].toInt());
    m_lateToleranceSpin->setValue(opts["lateTolerance"].toInt());
    m_concurrencySpin->setValue(opts["concurrency"].toInt());
    m_bufferLengthSpin->setValue(opts["bufferLength"].toInt());
    m_checkpointSpin->setValue(opts["checkpointInterval"].toInt());
    m_sendMetaCheck->setChecked(opts["sendMetaToSink"].toBool());
    m_sendErrorCheck->setChecked(opts["sendError"].toBool());

    // Sinks and Sink Options
    QJsonArray actions = data["actions"].toArray();
    if (!actions.isEmpty()) {
        QJsonObject firstAction = actions.at(0).toObject();
        QString type;
        QJsonObject props;
        if (firstAction.contains("log")) { type = "log"; props = firstAction["log"].toObject(); m_sinkTypeCombo->setCurrentIndex(0); }
        else if (firstAction.contains("edgex")) { type = "edgex"; props = firstAction["edgex"].toObject(); m_sinkTypeCombo->setCurrentIndex(1); }
        else if (firstAction.contains("mqtt")) { type = "mqtt"; props = firstAction["mqtt"].toObject(); m_sinkTypeCombo->setCurrentIndex(2); }
        else if (firstAction.contains("rest")) { type = "rest"; props = firstAction["rest"].toObject(); m_sinkTypeCombo->setCurrentIndex(3); }

        if (type == "edgex") {
            m_edgexProtocolCombo->setCurrentText(props["protocol"].toString());
            m_edgexHostEdit->setText(props["host"].toString());
            m_edgexPortEdit->setText(QString::number(props["port"].toInt()));
            m_edgexTopicEdit->setText(props["topic"].toString());
            m_edgexMsgTypeCombo->setCurrentText(props["messageType"].toString());
        } else if (type == "mqtt") {
            m_mqttBrokerEdit->setText(props["server"].toString());
            m_mqttTopicEdit->setText(props["topic"].toString());
            m_mqttClientIdEdit->setText(props["clientId"].toString());
            m_mqttQosSpin->setValue(props["qos"].toInt());
            m_mqttUserEdit->setText(props["username"].toString());
            m_mqttPassEdit->setText(props["password"].toString());
            m_mqttRetainedCheck->setChecked(props["retained"].toBool());
        } else if (type == "rest") {
            m_restUrlEdit->setText(props["url"].toString());
            m_restMethodCombo->setCurrentText(props["method"].toString());
            m_restBodyTypeCombo->setCurrentText(props["bodyType"].toString());
            m_restTimeoutSpin->setValue(props["timeout"].toInt());
            m_restDebugRespCheck->setChecked(props["debugResp"].toBool());
        }

        // Common/Advanced Sink Props
        m_sendSingleCheck->setChecked(props["sendSingle"].toBool());
        m_dataTemplateEdit->setPlainText(props["dataTemplate"].toString());
        m_insecureSkipVerifyCheck->setChecked(props["insecureSkipVerify"].toBool());
        m_certPathEdit->setText(props["certificationPath"].toString());
        m_privKeyPathEdit->setText(props["privateKeyPath"].toString());
        m_rootCaPathEdit->setText(props["rootCaPath"].toString());

        m_sinkConcurrencySpin->setValue(props["concurrency"].toInt());
        m_sinkBufferLengthSpin->setValue(props["bufferLength"].toInt());
        m_retryIntervalSpin->setValue(props["retryInterval"].toInt());
        m_retryCountSpin->setValue(props["retryCount"].toInt());
        m_cacheLengthSpin->setValue(props["cacheLength"].toInt());
        m_cacheSaveIntervalSpin->setValue(props["cacheSaveInterval"].toInt());
        m_runAsyncCheck->setChecked(props["runAsync"].toBool());
        m_omitIfEmptyCheck->setChecked(props["omitIfEmpty"].toBool());
    }
    
    setWindowTitle("Edit eKuiper Rule: " + id);
    m_btnOk->setText("Save Changes");
}
