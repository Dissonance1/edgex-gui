#include "EdgexClientBase.h"
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

EdgexClientBase::EdgexClientBase(QObject *parent)
    : QObject(parent), m_networkManager(new QNetworkAccessManager(this))
{
}

void EdgexClientBase::setBaseUrl(const QString &url)
{
    m_baseUrl = url;
    if (!m_baseUrl.endsWith("/")) {
        m_baseUrl += "/";
    }
}

QString EdgexClientBase::baseUrl() const
{
    return m_baseUrl;
}

QNetworkReply* EdgexClientBase::get(const QString &path)
{
    QString fullPath = m_baseUrl;
    if (!fullPath.endsWith('/') && !path.startsWith('/')) {
        fullPath += "/";
    }
    QUrl url(fullPath + path);
    QNetworkRequest request(url);
    return m_networkManager->get(request);
}

void EdgexClientBase::post(const QString &path, const QJsonObject &data, std::function<void(const QByteArray&, bool, const QString&)> callback)
{
    QString fullPath = m_baseUrl;
    if (!fullPath.endsWith('/') && !path.startsWith('/')) {
        fullPath += "/";
    }
    QNetworkRequest request(QUrl(fullPath + path));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply *reply = m_networkManager->post(request, QJsonDocument(data).toJson());
    connect(reply, &QNetworkReply::finished, this, [reply, callback]() {
        bool success = (reply->error() == QNetworkReply::NoError);
        QByteArray responseData = reply->readAll();
        QString errorMsg = success ? "" : reply->errorString();
        
        if (!success && !responseData.isEmpty()) {
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            if (doc.isObject() && doc.object().contains("message")) {
                errorMsg = doc.object()["message"].toString();
            }
        }
        callback(responseData, success, errorMsg);
        reply->deleteLater();
    });
}

void EdgexClientBase::post(const QString &path, const QJsonArray &data, std::function<void(const QByteArray&, bool, const QString&)> callback)
{
    QString fullPath = m_baseUrl;
    if (!fullPath.endsWith('/') && !path.startsWith('/')) {
        fullPath += "/";
    }
    QNetworkRequest request(QUrl(fullPath + path));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply *reply = m_networkManager->post(request, QJsonDocument(data).toJson());
    connect(reply, &QNetworkReply::finished, this, [reply, callback]() {
        bool success = (reply->error() == QNetworkReply::NoError);
        QByteArray responseData = reply->readAll();
        QString errorMsg = success ? "" : reply->errorString();
        if (!success && !responseData.isEmpty()) {
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            if (doc.isObject() && doc.object().contains("message")) {
                errorMsg = doc.object()["message"].toString();
            }
        }
        callback(responseData, success, errorMsg);
        reply->deleteLater();
    });
}

void EdgexClientBase::deleteRequest(const QString &path, std::function<void(const QByteArray&, bool, const QString&)> callback)
{
    QString fullPath = m_baseUrl;
    if (!fullPath.endsWith('/') && !path.startsWith('/')) {
        fullPath += "/";
    }
    QNetworkRequest request(QUrl(fullPath + path));
    
    QNetworkReply *reply = m_networkManager->deleteResource(request);
    connect(reply, &QNetworkReply::finished, this, [reply, callback]() {
        bool success = (reply->error() == QNetworkReply::NoError);
        QByteArray responseData = reply->readAll();
        QString errorMsg = success ? "" : reply->errorString();
        if (!success && !responseData.isEmpty()) {
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            if (doc.isObject() && doc.object().contains("message")) {
                errorMsg = doc.object()["message"].toString();
            }
        }
        callback(responseData, success, errorMsg);
        reply->deleteLater();
    });
}

void EdgexClientBase::put(const QString &path, const QJsonObject &data, std::function<void(const QByteArray&, bool, const QString&)> callback)
{
    QString fullPath = m_baseUrl;
    if (!fullPath.endsWith('/') && !path.startsWith('/')) {
        fullPath += "/";
    }
    QNetworkRequest request(QUrl(fullPath + path));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply *reply = m_networkManager->put(request, QJsonDocument(data).toJson());
    connect(reply, &QNetworkReply::finished, this, [reply, callback]() {
        bool success = (reply->error() == QNetworkReply::NoError);
        QByteArray responseData = reply->readAll();
        QString errorMsg = success ? "" : reply->errorString();
        if (!success && !responseData.isEmpty()) {
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            if (doc.isObject() && doc.object().contains("message")) {
                errorMsg = doc.object()["message"].toString();
            }
        }
        callback(responseData, success, errorMsg);
        reply->deleteLater();
    });
}

void EdgexClientBase::put(const QString &path, const QJsonArray &data, std::function<void(const QByteArray&, bool, const QString&)> callback)
{
    QString fullPath = m_baseUrl;
    if (!fullPath.endsWith('/') && !path.startsWith('/')) {
        fullPath += "/";
    }
    QNetworkRequest request(QUrl(fullPath + path));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply *reply = m_networkManager->put(request, QJsonDocument(data).toJson());
    connect(reply, &QNetworkReply::finished, this, [reply, callback]() {
        bool success = (reply->error() == QNetworkReply::NoError);
        QByteArray responseData = reply->readAll();
        QString errorMsg = success ? "" : reply->errorString();
        if (!success && !responseData.isEmpty()) {
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            if (doc.isObject() && doc.object().contains("message")) {
                errorMsg = doc.object()["message"].toString();
            }
        }
        callback(responseData, success, errorMsg);
        reply->deleteLater();
    });
}

void EdgexClientBase::patch(const QString &path, const QJsonObject &data, std::function<void(const QByteArray&, bool, const QString&)> callback)
{
    QString fullPath = m_baseUrl;
    if (!fullPath.endsWith('/') && !path.startsWith('/')) {
        fullPath += "/";
    }
    QNetworkRequest request(QUrl(fullPath + path));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply *reply = m_networkManager->sendCustomRequest(request, "PATCH", QJsonDocument(data).toJson());
    connect(reply, &QNetworkReply::finished, this, [reply, callback]() {
        bool success = (reply->error() == QNetworkReply::NoError);
        QByteArray responseData = reply->readAll();
        QString errorMsg = success ? "" : reply->errorString();
        if (!success && !responseData.isEmpty()) {
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            if (doc.isObject() && doc.object().contains("message")) {
                errorMsg = doc.object()["message"].toString();
            }
        }
        callback(responseData, success, errorMsg);
        reply->deleteLater();
    });
}

void EdgexClientBase::patch(const QString &path, const QJsonArray &data, std::function<void(const QByteArray&, bool, const QString&)> callback)
{
    QString fullPath = m_baseUrl;
    if (!fullPath.endsWith('/') && !path.startsWith('/')) {
        fullPath += "/";
    }
    QNetworkRequest request(QUrl(fullPath + path));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QNetworkReply *reply = m_networkManager->sendCustomRequest(request, "PATCH", QJsonDocument(data).toJson());
    connect(reply, &QNetworkReply::finished, this, [reply, callback]() {
        bool success = (reply->error() == QNetworkReply::NoError);
        QByteArray responseData = reply->readAll();
        QString errorMsg = success ? "" : reply->errorString();
        if (!success && !responseData.isEmpty()) {
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            if (doc.isObject() && doc.object().contains("message")) {
                errorMsg = doc.object()["message"].toString();
            }
        }
        callback(responseData, success, errorMsg);
        reply->deleteLater();
    });
}

void EdgexClientBase::postMultipart(const QString &path, QHttpMultiPart *multiPart, std::function<void(const QByteArray&, bool, const QString&)> callback)
{
    QString fullPath = m_baseUrl;
    if (!fullPath.endsWith('/') && !path.startsWith('/')) {
        fullPath += "/";
    }
    QNetworkRequest request(QUrl(fullPath + path));
    
    QNetworkReply *reply = m_networkManager->post(request, multiPart);
    multiPart->setParent(reply); // Ensure deletion

    connect(reply, &QNetworkReply::finished, this, [reply, callback]() {
        bool success = (reply->error() == QNetworkReply::NoError);
        QByteArray responseData = reply->readAll();
        QString errorMsg = success ? "" : reply->errorString();
        if (!success && !responseData.isEmpty()) {
            QJsonDocument doc = QJsonDocument::fromJson(responseData);
            if (doc.isObject() && doc.object().contains("message")) {
                errorMsg = doc.object()["message"].toString();
            }
        }
        callback(responseData, success, errorMsg);
        reply->deleteLater();
    });
}
