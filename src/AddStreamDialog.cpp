#include "AddStreamDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonArray>

AddStreamDialog::AddStreamDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

void AddStreamDialog::setupUi()
{
    setWindowTitle("Add eKuiper Stream (Enhanced)");
    resize(700, 500);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    QHBoxLayout *header = new QHBoxLayout();
    header->addWidget(new QLabel("Stream Preset:"));
    QComboBox *presets = new QComboBox();
    presets->addItem("EdgeX (Standard JSON)");
    presets->addItem("EdgeX (Custom/Extended)");
    presets->addItem("REST (HTTP Pull)");
    presets->addItem("MQTT Source");
    header->addWidget(presets);

    QPushButton *btnFormat = new QPushButton("Format SQL");
    header->addWidget(btnFormat);
    
    mainLayout->addLayout(header);

    mainLayout->addWidget(new QLabel("Stream SQL (CREATE STREAM syntax):"));

    m_sqlEdit = new QTextEdit();
    m_sqlEdit->setStyleSheet("font-family: 'Consolas', monospace; background-color: #0f172a; color: #4ade80; font-size: 13px;");
    mainLayout->addWidget(m_sqlEdit);

    auto updateSql = [this](int index) {
        if (index == 0) {
            m_sqlEdit->setPlainText("CREATE STREAM EdgeXStream () WITH ( FORMAT = \"JSON\", TYPE = \"edgex\" )");
        } else if (index == 1) {
            m_sqlEdit->setPlainText("CREATE STREAM CustomStream ( field1 bigint, field2 float ) \nWITH ( \n  DATASOURCE = \"\", \n  FORMAT = \"JSON\", \n  TYPE = \"edgex\", \n  STRICT_VALIDATION = \"true\", \n  SHARED = \"false\" \n)");
        } else if (index == 2) {
            m_sqlEdit->setPlainText("CREATE STREAM rest_stream () \nWITH ( \n  FORMAT = \"JSON\", \n  TYPE = \"httppull\", \n  URL = \"http://localhost:59882/api/v3/event/all\", \n  INTERVAL = \"5000\" \n)");
        } else {
            m_sqlEdit->setPlainText("CREATE STREAM mqtt_stream () \nWITH ( \n  FORMAT = \"JSON\", \n  TYPE = \"mqtt\", \n  DATASOURCE = \"manual/topic\" \n)");
        }
    };

    updateSql(0);
    connect(presets, QOverload<int>::of(&QComboBox::currentIndexChanged), updateSql);
    
    connect(btnFormat, &QPushButton::clicked, [this]() {
        QString sql = m_sqlEdit->toPlainText();
        // Very basic formatting: add newline before WITH and between WITH options
        sql.replace(" WITH (", "\nWITH (");
        sql.replace(", ", ",\n  ");
        m_sqlEdit->setPlainText(sql);
    });

    QHBoxLayout *buttons = new QHBoxLayout();
    QPushButton *btnOk = new QPushButton("Submit");
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

void AddStreamDialog::setSql(const QString &sql)
{
    m_sqlEdit->setPlainText(sql);
}

QString AddStreamDialog::streamJsonToSql(const QJsonObject &json)
{
    QString name = json["Name"].toString();
    QStringList fields;
    QJsonArray fieldArray = json["StreamFields"].toArray();
    for (int i = 0; i < fieldArray.size(); ++i) {
        QJsonObject f = fieldArray[i].toObject();
        fields.append(f["Name"].toString() + " " + f["FieldType"].toString());
    }

    QStringList options;
    QJsonObject optsObj = json["Options"].toObject();
    QStringList keys = optsObj.keys();
    for (const QString &key : keys) {
        options.append(key + " = \"" + optsObj[key].toVariant().toString() + "\"");
    }

    QString sql = QString("CREATE STREAM %1 (%2) WITH (%3)")
                      .arg(name)
                      .arg(fields.join(", "))
                      .arg(options.join(", "));
    return sql;
}
