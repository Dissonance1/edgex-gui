#include "AddSubscriptionDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QLabel>

AddSubscriptionDialog::AddSubscriptionDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

void AddSubscriptionDialog::setupUi()
{
    setWindowTitle("Add EdgeX Subscription (High-Fidelity V3)");
    resize(600, 650);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QTabWidget *tabs = new QTabWidget();
    
    // TAB 1: General & Delivery
    QWidget *genTab = new QWidget();
    QVBoxLayout *genLayout = new QVBoxLayout(genTab);
    QFormLayout *form = new QFormLayout();

    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText("e.g. device-alerts");
    form->addRow("Subscription Name*:", m_nameEdit);

    m_descriptionEdit = new QLineEdit();
    form->addRow("Description:", m_descriptionEdit);

    m_receiverEdit = new QLineEdit();
    m_receiverEdit->setPlaceholderText("e.g. admin-group");
    form->addRow("Receiver*:", m_receiverEdit);

    m_adminStateCombo = new QComboBox();
    m_adminStateCombo->addItems({"UNLOCKED", "LOCKED"});
    form->addRow("Admin State:", m_adminStateCombo);

    m_categoriesEdit = new QLineEdit();
    m_categoriesEdit->setPlaceholderText("Comma separated: HW_ALERTS, SW_ALERTS");
    form->addRow("Categories:", m_categoriesEdit);

    m_labelsEdit = new QLineEdit();
    m_labelsEdit->setPlaceholderText("Comma separated tagging");
    form->addRow("Labels:", m_labelsEdit);

    form->addRow(new QLabel("<b>Delivery Options</b>"), new QLabel(""));
    
    m_resendIntervalEdit = new QLineEdit("1s");
    m_resendIntervalEdit->setPlaceholderText("e.g. 100ms, 1s, 24h");
    form->addRow("Resend Interval:", m_resendIntervalEdit);

    m_resendLimitSpin = new QSpinBox();
    m_resendLimitSpin->setRange(1, 1000);
    m_resendLimitSpin->setValue(10);
    form->addRow("Resend Limit:", m_resendLimitSpin);

    genLayout->addLayout(form);
    genLayout->addStretch();
    tabs->addTab(genTab, "General");

    // TAB 2: Channels
    tabs->addTab(createChannelBuilder(), "Channels");
    
    mainLayout->addWidget(tabs);

    QHBoxLayout *buttons = new QHBoxLayout();
    QPushButton *btnOk = new QPushButton("Create Subscription");
    QPushButton *btnCancel = new QPushButton("Cancel");
    buttons->addStretch();
    buttons->addWidget(btnOk);
    buttons->addWidget(btnCancel);
    mainLayout->addLayout(buttons);

    connect(btnOk, &QPushButton::clicked, [this]() {
        if (m_nameEdit->text().isEmpty() || m_receiverEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Validation", "Name and Receiver are required.");
            return;
        }
        accept();
    });
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

QWidget* AddSubscriptionDialog::createChannelBuilder()
{
    QWidget *w = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(w);

    QHBoxLayout *header = new QHBoxLayout();
    header->addWidget(new QLabel("Channel Type:"));
    m_channelTypeCombo = new QComboBox();
    m_channelTypeCombo->addItems({"REST", "EMAIL"});
    header->addWidget(m_channelTypeCombo);
    header->addStretch();
    layout->addLayout(header);

    m_channelStack = new QStackedWidget();

    // 0: REST
    QWidget *restW = new QWidget();
    QFormLayout *restF = new QFormLayout(restW);
    m_restMethodCombo = new QComboBox();
    m_restMethodCombo->addItems({"POST", "PUT", "GET", "PATCH"});
    restF->addRow("HTTP Method:", m_restMethodCombo);
    
    m_restHostEdit = new QLineEdit("localhost");
    restF->addRow("Host*:", m_restHostEdit);
    
    m_restPortEdit = new QLineEdit("8080");
    restF->addRow("Port*:", m_restPortEdit);
    
    m_restPathEdit = new QLineEdit("/api/v1/notification");
    restF->addRow("Path:", m_restPathEdit);
    m_channelStack->addWidget(restW);

    // 1: EMAIL
    QWidget *emailW = new QWidget();
    QFormLayout *emailF = new QFormLayout(emailW);
    m_emailRecipientsEdit = new QLineEdit();
    m_emailRecipientsEdit->setPlaceholderText("user1@example.com, user2@example.com");
    emailF->addRow("Recipients*:", m_emailRecipientsEdit);
    m_channelStack->addWidget(emailW);

    layout->addWidget(m_channelStack);
    connect(m_channelTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), m_channelStack, &QStackedWidget::setCurrentIndex);

    layout->addStretch();
    return w;
}

QJsonObject AddSubscriptionDialog::subscriptionData() const
{
    QJsonObject sub;
    sub["name"] = m_nameEdit->text();
    sub["description"] = m_descriptionEdit->text();
    sub["receiver"] = m_receiverEdit->text();
    sub["adminState"] = m_adminStateCombo->currentText();
    sub["resendInterval"] = m_resendIntervalEdit->text();
    sub["resendLimit"] = m_resendLimitSpin->value();

    // Categories
    if (!m_categoriesEdit->text().isEmpty()) {
        QJsonArray cats;
        for (const QString &s : m_categoriesEdit->text().split(",", Qt::SkipEmptyParts)) {
            cats.append(s.trimmed());
        }
        sub["categories"] = cats;
    }

    // Labels
    if (!m_labelsEdit->text().isEmpty()) {
        QJsonArray labs;
        for (const QString &s : m_labelsEdit->text().split(",", Qt::SkipEmptyParts)) {
            labs.append(s.trimmed());
        }
        sub["labels"] = labs;
    }

    // Channel
    QJsonObject channel;
    QString type = m_channelTypeCombo->currentText();
    channel["type"] = type;
    
    if (type == "REST") {
        channel["httpMethod"] = m_restMethodCombo->currentText();
        channel["host"] = m_restHostEdit->text();
        channel["port"] = m_restPortEdit->text().toInt();
        channel["path"] = m_restPathEdit->text();
    } else if (type == "EMAIL") {
        QJsonArray recipients;
        for (const QString &r : m_emailRecipientsEdit->text().split(",", Qt::SkipEmptyParts)) {
            recipients.append(r.trimmed());
        }
        channel["emailRecipients"] = recipients;
    }

    QJsonArray channels;
    channels.append(channel);
    sub["channels"] = channels;

    QJsonObject request;
    request["apiVersion"] = "v3";
    request["subscription"] = sub;
    
    return request;
}
