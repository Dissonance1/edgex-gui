#ifndef ADDSTREAMDIALOG_H
#define ADDSTREAMDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>

class AddStreamDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddStreamDialog(QWidget *parent = nullptr);
    QString sql() const;

private:
    void setupUi();
    QTextEdit *m_sqlEdit;
};

#endif // ADDSTREAMDIALOG_H
