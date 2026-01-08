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

class AddRuleDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddRuleDialog(QWidget *parent = nullptr);
    QString id() const;
    QJsonObject ruleData() const;

private slots:
    void onSinkTypeChanged(int index);

private:
    void setupUi();
    QWidget* createGeneralTab();
    QWidget* createSinksTab();
    QWidget* createOptionsTab();

    // General
    QLineEdit *m_idEdit;
    QTextEdit *m_sqlEdit;

    // Sinks
    QComboBox *m_sinkTypeCombo;
    QStackedWidget *m_sinkStack;
    
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

    // REST Sink Fields
    QLineEdit *m_restUrlEdit;
    QComboBox *m_restMethodCombo;

    // Options
    QCheckBox *m_debugCheck;
    QCheckBox *m_logPlanCheck;
    QCheckBox *m_sendMetaCheck;
    QSpinBox *m_qosSpin;
    QSpinBox *m_checkpointSpin;
};

#endif // ADDRULEDIALOG_H
