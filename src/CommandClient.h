#ifndef COMMANDCLIENT_H
#define COMMANDCLIENT_H

#include "EdgexClientBase.h"
#include <QJsonArray>
#include <QJsonObject>

class CommandClient : public EdgexClientBase
{
    Q_OBJECT
public:
    explicit CommandClient(QObject *parent = nullptr);

    void fetchDeviceCommands(const QString &deviceName);
    void executeCommand(const QString &deviceName, const QString &commandName, const QString &method, const QJsonObject &params = QJsonObject());

signals:
    void commandsReceived(const QJsonArray &commands);
    void commandExecuted(bool success, const QString &message);
};

#endif // COMMANDCLIENT_H
