#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>
#include <QSettings>
#include <QDir>
#include <QStandardPaths>

class ConfigManager
{
public:
    static ConfigManager& instance() {
        static ConfigManager inst;
        return inst;
    }

    void load();
    void save();

    QString metadataUrl() const { return m_metadataUrl; }
    void setMetadataUrl(const QString &url) { m_metadataUrl = url; }

    QString coreDataUrl() const { return m_coreDataUrl; }
    void setCoreDataUrl(const QString &url) { m_coreDataUrl = url; }

    QString commandUrl() const { return m_commandUrl; }
    void setCommandUrl(const QString &url) { m_commandUrl = url; }

    QString systemUrl() const { return m_systemUrl; }
    void setSystemUrl(const QString &url) { m_systemUrl = url; }

    QString notificationsUrl() const { return m_notificationsUrl; }
    void setNotificationsUrl(const QString &url) { m_notificationsUrl = url; }

    QString schedulerUrl() const { return m_schedulerUrl; }
    void setSchedulerUrl(const QString &url) { m_schedulerUrl = url; }

    QString rulesUrl() const { return m_rulesUrl; }
    void setRulesUrl(const QString &url) { m_rulesUrl = url; }

    QString loggingUrl() const { return m_loggingUrl; }
    void setLoggingUrl(const QString &url) { m_loggingUrl = url; }

    QString registryUrl() const { return m_registryUrl; }
    void setRegistryUrl(const QString &url) { m_registryUrl = url; }

private:
    ConfigManager() = default;
    
    QString m_metadataUrl = "http://localhost:59881";
    QString m_coreDataUrl = "http://localhost:59880";
    QString m_commandUrl = "http://localhost:59882";
    QString m_systemUrl = "http://localhost:59890";
    QString m_notificationsUrl = "http://localhost:59860";
    QString m_schedulerUrl = "http://localhost:59861";
    QString m_rulesUrl = "http://localhost:9081";
    QString m_loggingUrl = "http://localhost:59861";
    QString m_registryUrl = "http://localhost:8500";
};

#endif // CONFIGMANAGER_H
