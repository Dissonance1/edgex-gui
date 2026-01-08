#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLineEdit>

class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);

private slots:
    void onSave();

private:
    void setupUi();
    
    QLineEdit *m_metadataEdit;
    QLineEdit *m_dataEdit;
    QLineEdit *m_commandEdit;
    QLineEdit *m_systemEdit;
    QLineEdit *m_notificationsEdit;
    QLineEdit *m_schedulerEdit;
    QLineEdit *m_rulesEdit;
};

#endif // SETTINGSDIALOG_H
