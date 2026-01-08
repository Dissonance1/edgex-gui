#ifndef EDGEXCLIENTBASE_H
#define EDGEXCLIENTBASE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QHttpMultiPart>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class EdgexClientBase : public QObject
{
    Q_OBJECT
public:
    explicit EdgexClientBase(QObject *parent = nullptr);
    virtual ~EdgexClientBase() = default;

    void setBaseUrl(const QString &url);
    QString baseUrl() const;
    QNetworkReply* get(const QString &path);

protected:
    void post(const QString &path, const QJsonObject &data, std::function<void(const QByteArray&, bool, const QString&)> callback);
    void post(const QString &path, const QJsonArray &data, std::function<void(const QByteArray&, bool, const QString&)> callback);
    void put(const QString &path, const QJsonObject &data, std::function<void(const QByteArray&, bool, const QString&)> callback);
    void put(const QString &path, const QJsonArray &data, std::function<void(const QByteArray&, bool, const QString&)> callback);
    void patch(const QString &path, const QJsonObject &data, std::function<void(const QByteArray&, bool, const QString&)> callback);
    void patch(const QString &path, const QJsonArray &data, std::function<void(const QByteArray&, bool, const QString&)> callback);
    void postMultipart(const QString &path, QHttpMultiPart *multiPart, std::function<void(const QByteArray&, bool, const QString&)> callback);
    void deleteRequest(const QString &path, std::function<void(const QByteArray&, bool, const QString&)> callback);

    QNetworkAccessManager *m_networkManager;
    QString m_baseUrl;

signals:
    void errorOccurred(const QString &message);
};

#endif // EDGEXCLIENTBASE_H
