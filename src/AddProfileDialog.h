#ifndef ADDPROFILEDIALOG_H
#define ADDPROFILEDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>

class AddProfileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddProfileDialog(QWidget *parent = nullptr);
    QString yamlContent() const;
    void setYaml(const QString &yaml);
    void setActionButtonText(const QString &text);

private slots:
    void onBrowse();

private:
    void setupUi();
    QTextEdit *m_yamlEdit;
    QPushButton *m_btnOk;
};

#endif // ADDPROFILEDIALOG_H
