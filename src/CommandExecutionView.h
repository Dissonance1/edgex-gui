#ifndef COMMANDEXECUTIONVIEW_H
#define COMMANDEXECUTIONVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QJsonArray>
#include <QJsonObject>
#include <QMap>
#include "CommandClient.h"

class CommandExecutionView : public QWidget
{
    Q_OBJECT
public:
    explicit CommandExecutionView(const QString &deviceName, QWidget *parent = nullptr);

private slots:
    void onCommandsReceived(const QJsonArray &commands);
    void onCommandExecuted(bool success, const QString &message);
    void executeGet(const QString &commandName);
    void executePut(const QString &commandName, const QJsonArray &parameters);

private:
    void setupUi();
    void createCommandWidget(const QJsonObject &command);
    QWidget* createParameterWidget(const QJsonObject &param, const QString &cmdName);

    QString m_deviceName;
    CommandClient *m_client;
    QVBoxLayout *m_scrollLayout;
    QMap<QString, QLineEdit*> m_paramInputs;
};

#endif // COMMANDEXECUTIONVIEW_H
