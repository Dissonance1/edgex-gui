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
    QPushButton *btnOk = new QPushButton("Add");
    QPushButton *btnCancel = new QPushButton("Cancel");

    buttons->addWidget(btnBrowse);
    buttons->addStretch();
    buttons->addWidget(btnOk);
    buttons->addWidget(btnCancel);
    mainLayout->addLayout(buttons);

    connect(btnBrowse, &QPushButton::clicked, this, &AddProfileDialog::onBrowse);
    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);
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
