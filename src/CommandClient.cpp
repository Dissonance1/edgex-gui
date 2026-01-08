#include "CommandClient.h"
#include <QJsonDocument>

CommandClient::CommandClient(QObject *parent)
    : EdgexClientBase(parent)
{
}

void CommandClient::fetchDeviceCommands(const QString &deviceName)
{
    QNetworkReply *reply = get("/device/name/" + deviceName);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject obj = doc.object();
            // EdgeX V3 format: { "deviceCoreCommand": { "coreCommands": [...] } }
            QJsonObject coreCmdObj = obj["deviceCoreCommand"].toObject();
            emit commandsReceived(coreCmdObj["coreCommands"].toArray());
        }
        reply->deleteLater();
    });
}

void CommandClient::executeCommand(const QString &deviceName, const QString &commandName, const QString &method, const QJsonObject &params)
{
    QString path = QString("/device/name/%1/command/%2").arg(deviceName, commandName);
    
    auto callback = [this](const QByteArray &data, bool success, const QString &error) {
        emit commandExecuted(success, success ? "Command executed" : error);
    };

    if (method.toUpper() == "GET") {
        QNetworkReply *reply = get(path);
        connect(reply, &QNetworkReply::finished, this, [this, reply, callback]() {
            bool success = (reply->error() == QNetworkReply::NoError);
            callback(reply->readAll(), success, success ? "" : reply->errorString());
            reply->deleteLater();
        });
    } else {
        put(path, params, callback);
    }
}
