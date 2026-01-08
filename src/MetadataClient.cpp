#include "MetadataClient.h"

MetadataClient::MetadataClient(QObject *parent)
    : EdgexClientBase(parent)
{
}

void MetadataClient::fetchDevices()
{
    QNetworkReply *reply = get("api/v3/device/all");
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit devicesReceived(doc.object()["devices"].toArray());
        } else {
            emit errorOccurred(reply->errorString());
        }
        reply->deleteLater();
    });
}

void MetadataClient::fetchDeviceServices()
{
    QNetworkReply *reply = get("api/v3/deviceservice/all");
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit deviceServicesReceived(doc.object()["services"].toArray());
        } else {
            emit errorOccurred(reply->errorString());
        }
        reply->deleteLater();
    });
}

void MetadataClient::fetchDeviceProfiles()
{
    QNetworkReply *reply = get("api/v3/deviceprofile/all");
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit deviceProfilesReceived(doc.object()["profiles"].toArray());
        } else {
            emit errorOccurred(reply->errorString());
        }
        reply->deleteLater();
    });
}
void MetadataClient::addDevice(const QJsonObject &device)
{
    QJsonArray array;
    array.append(device);
    post("api/v3/device", array, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Device added successfully" : error);
    });
}

void MetadataClient::updateDevice(const QJsonObject &device)
{
    QJsonArray array;
    array.append(device);
    patch("api/v3/device", array, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Device updated successfully" : error);
    });
}

void MetadataClient::deleteDevice(const QString &deviceName)
{
    deleteRequest("api/v3/device/name/" + deviceName, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Device deleted successfully" : error);
    });
}

void MetadataClient::addProfile(const QJsonObject &profile)
{
    QJsonArray array;
    array.append(profile);
    post("api/v3/deviceprofile", array, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Profile added successfully" : error);
    });
}

void MetadataClient::addProfileFromYaml(const QString &yaml)
{
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"profile.yaml\""));
    filePart.setBody(yaml.toUtf8());
    multiPart->append(filePart);

    postMultipart("api/v3/deviceprofile/uploadfile", multiPart, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Profile uploaded successfully" : error);
    });
}

void MetadataClient::deleteProfile(const QString &profileName)
{
    deleteRequest("api/v3/deviceprofile/name/" + profileName, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Profile deleted successfully" : error);
    });
}

void MetadataClient::addDeviceService(const QJsonObject &service)
{
    QJsonArray array;
    array.append(service);
    post("api/v3/deviceservice", array, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Device Service added successfully" : error);
    });
}

void MetadataClient::updateDeviceService(const QJsonObject &service)
{
    QJsonArray array;
    array.append(service);
    patch("api/v3/deviceservice", array, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Device Service updated successfully" : error);
    });
}

void MetadataClient::deleteDeviceService(const QString &serviceName)
{
    deleteRequest("api/v3/deviceservice/name/" + serviceName, [this](const QByteArray &data, bool success, const QString &error) {
        emit operationCompleted(success, success ? "Device Service deleted successfully" : error);
    });
}
