#include "AddStreamDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>

AddStreamDialog::AddStreamDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

void AddStreamDialog::setupUi()
{
    setWindowTitle("Add eKuiper Stream");
    resize(600, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QHBoxLayout *header = new QHBoxLayout();
    header->addWidget(new QLabel("Stream Preset:"));
    QComboBox *presets = new QComboBox();
    presets->addItem("EdgeX (Dedicated Source)");
    presets->addItem("MQTT Fallback (For EdgeX via MQTT)");
    presets->addItem("HTTP Fallback");
    header->addWidget(presets);
    mainLayout->addLayout(header);

    mainLayout->addWidget(new QLabel("Stream SQL:"));

    m_sqlEdit = new QTextEdit();
    m_sqlEdit->setStyleSheet("font-family: 'Consolas', monospace; background-color: #0f172a; color: #4ade80;");
    mainLayout->addWidget(m_sqlEdit);

    auto updateSql = [this](int index) {
        if (index == 0) {
            m_sqlEdit->setPlainText("CREATE STREAM edgex_stream () WITH (FORMAT=\"JSON\", TYPE=\"edgex\")");
        } else if (index == 1) {
            m_sqlEdit->setPlainText("-- Use this if 'edgex' type is not found\nCREATE STREAM edgex_mqtt_stream () WITH (FORMAT=\"JSON\", TYPE=\"mqtt\", DATASOURCE=\"edgex/events\")");
        } else {
            m_sqlEdit->setPlainText("CREATE STREAM http_stream () WITH (FORMAT=\"JSON\", TYPE=\"httppull\", URL=\"http://localhost:59882/api/v3/event/all\")");
        }
    };

    updateSql(0);
    connect(presets, QOverload<int>::of(&QComboBox::currentIndexChanged), updateSql);

    QHBoxLayout *buttons = new QHBoxLayout();
    QPushButton *btnOk = new QPushButton("Create");
    QPushButton *btnCancel = new QPushButton("Cancel");
    buttons->addStretch();
    buttons->addWidget(btnOk);
    buttons->addWidget(btnCancel);
    mainLayout->addLayout(buttons);

    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

QString AddStreamDialog::sql() const
{
    return m_sqlEdit->toPlainText();
}
