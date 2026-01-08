#ifndef ADDSUBSCRIPTIONDIALOG_H
#define ADDSUBSCRIPTIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QStackedWidget>
#include <QSpinBox>

class AddSubscriptionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddSubscriptionDialog(QWidget *parent = nullptr);
    QJsonObject subscriptionData() const;

private:
    void setupUi();
    QWidget* createChannelBuilder();

    // General
    QLineEdit *m_nameEdit;
    QLineEdit *m_descriptionEdit;
    QLineEdit *m_receiverEdit;
    QLineEdit *m_categoriesEdit;
    QLineEdit *m_labelsEdit;
    QComboBox *m_adminStateCombo;
    
    // Delivery
    QLineEdit *m_resendIntervalEdit;
    QSpinBox *m_resendLimitSpin;

    // Channel Builder
    QComboBox *m_channelTypeCombo;
    QStackedWidget *m_channelStack;
    
    // REST Channel Specifics
    QComboBox *m_restMethodCombo;
    QLineEdit *m_restHostEdit;
    QLineEdit *m_restPortEdit;
    QLineEdit *m_restPathEdit;

    // EMAIL Channel Specifics
    QLineEdit *m_emailRecipientsEdit;
};

#endif // ADDSUBSCRIPTIONDIALOG_H
