#ifndef ADDRULEDIALOG_H
#define ADDRULEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QTabWidget>
#include <QCheckBox>
#include <QSpinBox>
#include <QStackedWidget>
#include <QJsonObject>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>

class AddRuleDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddRuleDialog(QWidget *parent = nullptr);
    QString id() const;
    QJsonObject ruleData() const;
    void setRuleData(const QString &id, const QJsonObject &data);

private slots:
    void onSinkTypeChanged(int index);

private:
    void setupUi();
    QWidget* createGeneralTab();
    QWidget* createSinksTab();
    QWidget* createRuleOptionsTab();
    QWidget* createSinkOptionsTab();

    // --- General Tab ---
    QLineEdit *m_idEdit;
    QTextEdit *m_sqlEdit;

    // --- Sinks Tab ---
    QComboBox *m_sinkTypeCombo;
    QStackedWidget *m_sinkStack;
    
    // Common Sink Properties (Data Template, etc.)
    QCheckBox *m_sendSingleCheck;
    QTextEdit *m_dataTemplateEdit;

    // EdgeX Sink Fields
    QComboBox *m_edgexProtocolCombo;
    QLineEdit *m_edgexHostEdit;
    QLineEdit *m_edgexPortEdit;
    QLineEdit *m_edgexTopicEdit;
    QComboBox *m_edgexMsgTypeCombo;

    // MQTT Sink Fields
    QLineEdit *m_mqttBrokerEdit;
    QLineEdit *m_mqttTopicEdit;
    QLineEdit *m_mqttClientIdEdit;
    QSpinBox *m_mqttQosSpin;
    QLineEdit *m_mqttUserEdit;
    QLineEdit *m_mqttPassEdit;
    QCheckBox *m_mqttRetainedCheck;

    // REST Sink Fields
    QLineEdit *m_restUrlEdit;
    QComboBox *m_restMethodCombo;
    QComboBox *m_restBodyTypeCombo;
    QSpinBox *m_restTimeoutSpin;
    QCheckBox *m_restDebugRespCheck;

    // Security (Shared by REST/MQTT)
    QCheckBox *m_insecureSkipVerifyCheck;
    QLineEdit *m_certPathEdit;
    QLineEdit *m_privKeyPathEdit;
    QLineEdit *m_rootCaPathEdit;

    // --- Rule Options Tab ---
    QCheckBox *m_isEventTimeCheck;
    QSpinBox *m_qosSpin;
    QSpinBox *m_lateToleranceSpin;
    QSpinBox *m_concurrencySpin;
    QSpinBox *m_bufferLengthSpin;
    QSpinBox *m_checkpointSpin;
    QCheckBox *m_sendMetaCheck;
    QCheckBox *m_sendErrorCheck;

    // --- Sink Options Tab ---
    QSpinBox *m_sinkConcurrencySpin;
    QSpinBox *m_sinkBufferLengthSpin;
    QSpinBox *m_retryIntervalSpin;
    QSpinBox *m_retryCountSpin;
    QSpinBox *m_cacheLengthSpin;
    QSpinBox *m_cacheSaveIntervalSpin;
    QCheckBox *m_runAsyncCheck;
    QCheckBox *m_omitIfEmptyCheck;

    QPushButton *m_btnOk;
};

#endif // ADDRULEDIALOG_H
