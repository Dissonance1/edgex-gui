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

private slots:
    void onBrowse();

private:
    void setupUi();
    QTextEdit *m_yamlEdit;
};

#endif // ADDPROFILEDIALOG_H
