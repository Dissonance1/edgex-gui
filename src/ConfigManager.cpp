#include "ConfigManager.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QStandardPaths> // Added this include as it's used but not present in the original snippet
#include <QDir> // Added this include as it's used but not present in the original snippet

void ConfigManager::load()
{
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/config.json";
    QFile file(configPath);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject obj = doc.object();
        if (obj.contains("metadataUrl")) m_metadataUrl = obj["metadataUrl"].toString();
        if (obj.contains("coreDataUrl")) m_coreDataUrl = obj["coreDataUrl"].toString();
        if (obj.contains("commandUrl")) m_commandUrl = obj["commandUrl"].toString();
        if (obj.contains("systemUrl")) m_systemUrl = obj["systemUrl"].toString();
        if (obj.contains("notificationsUrl")) m_notificationsUrl = obj["notificationsUrl"].toString();
        if (obj.contains("schedulerUrl")) m_schedulerUrl = obj["schedulerUrl"].toString();
        if (obj.contains("rulesUrl")) m_rulesUrl = obj["rulesUrl"].toString();
        if (obj.contains("loggingUrl")) m_loggingUrl = obj["loggingUrl"].toString();
        if (obj.contains("registryUrl")) m_registryUrl = obj["registryUrl"].toString();
    }
}

void ConfigManager::save()
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configDir);
    
    QFile file(configDir + "/config.json");
    if (file.open(QIODevice::WriteOnly)) {
        QJsonObject obj;
        obj["metadataUrl"] = m_metadataUrl;
        obj["coreDataUrl"] = m_coreDataUrl;
        obj["commandUrl"] = m_commandUrl;
        obj["systemUrl"] = m_systemUrl;
        obj["notificationsUrl"] = m_notificationsUrl;
        obj["schedulerUrl"] = m_schedulerUrl;
        obj["rulesUrl"] = m_rulesUrl;
        obj["loggingUrl"] = m_loggingUrl;
        obj["registryUrl"] = m_registryUrl;
        file.write(QJsonDocument(obj).toJson());
    }
}
