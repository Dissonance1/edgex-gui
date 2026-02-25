#include "AddProfileDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QLabel>

AddProfileDialog::AddProfileDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

void AddProfileDialog::setupUi()
{
    setWindowTitle("Add Device Profile");
    resize(600, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(new QLabel("Paste YAML content or browse for a file:"));

    m_yamlEdit = new QTextEdit();
    m_yamlEdit->setAcceptRichText(false);
    mainLayout->addWidget(m_yamlEdit);

    QHBoxLayout *buttons = new QHBoxLayout();
    QPushButton *btnBrowse = new QPushButton("Browse...");
    m_btnOk = new QPushButton("Add");
    QPushButton *btnCancel = new QPushButton("Cancel");

    buttons->addWidget(btnBrowse);
    buttons->addStretch();
    buttons->addWidget(m_btnOk);
    buttons->addWidget(btnCancel);
    mainLayout->addLayout(buttons);

    connect(btnBrowse, &QPushButton::clicked, this, &AddProfileDialog::onBrowse);
    connect(m_btnOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

void AddProfileDialog::onBrowse()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Profile YAML", "", "YAML Files (*.yaml *.yml);;All Files (*)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            m_yamlEdit->setPlainText(file.readAll());
        }
    }
}

QString AddProfileDialog::yamlContent() const
{
    return m_yamlEdit->toPlainText();
}

void AddProfileDialog::setYaml(const QString &yaml)
{
    m_yamlEdit->setPlainText(yaml);
    setWindowTitle("Edit Device Profile");
}

void AddProfileDialog::setActionButtonText(const QString &text)
{
    m_btnOk->setText(text);
}
