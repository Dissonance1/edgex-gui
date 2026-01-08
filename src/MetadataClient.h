#ifndef METADATACLIENT_H
#define METADATACLIENT_H

#include "EdgexClientBase.h"

class MetadataClient : public EdgexClientBase
{
    Q_OBJECT
public:
    explicit MetadataClient(QObject *parent = nullptr);

    void fetchDevices();
    void fetchDeviceServices();
    void fetchDeviceProfiles();

    void addDevice(const QJsonObject &device);
    void updateDevice(const QJsonObject &device);
    void deleteDevice(const QString &deviceName);

    void addProfile(const QJsonObject &profile);
    void addProfileFromYaml(const QString &yaml);
    void deleteProfile(const QString &profileName);

    void addDeviceService(const QJsonObject &service);
    void updateDeviceService(const QJsonObject &service);
    void deleteDeviceService(const QString &serviceName);

signals:
    void devicesReceived(const QJsonArray &devices);
    void deviceServicesReceived(const QJsonArray &services);
    void deviceProfilesReceived(const QJsonArray &profiles);
    void operationCompleted(bool success, const QString &message);
};

#endif // METADATACLIENT_H
