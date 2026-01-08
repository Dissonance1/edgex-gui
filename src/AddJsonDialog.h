#ifndef ADDJSONDIALOG_H
#define ADDJSONDIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

class AddJsonDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddJsonDialog(const QString &title, const QString &placeholder = "{}", QWidget *parent = nullptr)
        : QDialog(parent) {
        setWindowTitle(title);
        resize(500, 400);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(new QLabel("Enter JSON Payload:"));

        m_textEdit = new QPlainTextEdit();
        m_textEdit->setPlainText(placeholder);
        m_textEdit->setStyleSheet("font-family: monospace;");
        layout->addWidget(m_textEdit);

        QHBoxLayout *buttons = new QHBoxLayout();
        QPushButton *btnOk = new QPushButton("Submit");
        QPushButton *btnCancel = new QPushButton("Cancel");
        buttons->addStretch();
        buttons->addWidget(btnOk);
        buttons->addWidget(btnCancel);
        layout->addLayout(buttons);

        connect(btnOk, &QPushButton::clicked, this, [this]() {
            QJsonDocument doc = QJsonDocument::fromJson(m_textEdit->toPlainText().toUtf8());
            if (doc.isNull()) {
                QMessageBox::warning(this, "Invalid JSON", "Please enter a valid JSON object.");
                return;
            }
            accept();
        });
        connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    }

    QJsonObject jsonObject() const {
        return QJsonDocument::fromJson(m_textEdit->toPlainText().toUtf8()).object();
    }

private:
    QPlainTextEdit *m_textEdit;
};

#endif // ADDJSONDIALOG_H
