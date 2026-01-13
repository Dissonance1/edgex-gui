#include "DataIngestionServer.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>

DataIngestionServer::DataIngestionServer(QObject *parent)
    : QObject(parent)
    , m_server(new QTcpServer(this))
    , m_networkManager(new QNetworkAccessManager(this))
{
    connect(m_server, &QTcpServer::newConnection, this, &DataIngestionServer::onNewConnection);
}

bool DataIngestionServer::listen(quint16 port)
{
    if (m_server->listen(QHostAddress::AnyIPv4, port)) {
        qInfo() << "DataIngestionServer listening on port" << port;
        return true;
    }
    qWarning() << "DataIngestionServer failed to listen on port" << port << ":" << m_server->errorString();
    return false;
}

void DataIngestionServer::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, this, &DataIngestionServer::onReadyRead);
        connect(socket, &QTcpSocket::disconnected, this, &DataIngestionServer::onClientDisconnected);
    }
}

void DataIngestionServer::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    
    RequestContext &ctx = m_requests[socket];
    ctx.buffer.append(socket->readAll());
    
    processRequest(socket, ctx.buffer);
}

void DataIngestionServer::onClientDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (socket) {
        m_requests.remove(socket);
        socket->deleteLater();
    }
}

void DataIngestionServer::processRequest(QTcpSocket* socket, const QByteArray& data)
{
    RequestContext &ctx = m_requests[socket];
    
    if (!ctx.headersParsed) {
        int splitIndex = data.indexOf("\r\n\r\n");
        if (splitIndex == -1) return; // Wait for more data
        
        ctx.headersParsed = true;
        QByteArray headers = data.left(splitIndex);
        
        // Parse Content-Length
        QList<QByteArray> lines = headers.split('\n');
        for (const QByteArray &line : lines) {
            QString l = QString::fromUtf8(line).toLower();
            if (l.startsWith("content-length:")) {
                ctx.contentLength = l.mid(15).trimmed().toLongLong();
                break;
            }
        }
        
        if (ctx.contentLength == -1) {
            if (headers.contains("POST")) {
                 qWarning() << "POST without Content-Length";
            }
            ctx.contentLength = 0; 
        }
    }
    
    int splitIndex = data.indexOf("\r\n\r\n");
    QByteArray body = data.mid(splitIndex + 4);
    
    if (body.size() < ctx.contentLength) {
        return; // Wait for full body
    }
    
    // We have the full request!
    QByteArray headerPart = data.left(splitIndex);
    
    // Transparent Proxy Logic with SenML Translation
    if (headerPart.contains("POST")) {
        QString headerLine = QString::fromUtf8(headerPart.split('\n')[0]);
        QStringList parts = headerLine.split(' ');
        if (parts.size() < 2) {
             m_requests.remove(socket);
             return;
        }
        
        QString requestPath = parts[1];
        
        if (requestPath.startsWith("/core-data")) {
             QString forwardingPath = requestPath.mid(10); // Remove "/core-data"
             QUrl url;
             
             // Mimic edgex-ui-go: Pure Transparent Proxy
             // Strip /core-data and forward exactly as-is to port 59880
             url = QUrl("http://localhost:59880" + forwardingPath);
             
             qInfo() << "Proxying" << requestPath << "->" << url.toString();
             
             QNetworkRequest request(url);
             request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
             
             QNetworkReply *reply = m_networkManager->post(request, body);
             
             connect(reply, &QNetworkReply::finished, this, [this, reply, socket, url]() {
                if (socket->state() == QAbstractSocket::ConnectedState) {
                    QByteArray responseData = reply->readAll();
                    if (reply->error() == QNetworkReply::NoError) {
                        qInfo() << "Forward Success:" << url.toString() << responseData;
                        sendResponse(socket, 200, responseData);
                    } else {
                        qWarning() << "Forward Error:" << reply->errorString() << responseData;
                        sendResponse(socket, 500, responseData.isEmpty() ? reply->errorString().toUtf8() : responseData);
                    }
                }
                m_requests.remove(socket);
                reply->deleteLater();
             });
             return;
        }
    }
    
    qWarning() << "Unmatched Request:" << headerPart.left(100);
    sendResponse(socket, 404, "Not Found");
    m_requests.remove(socket);
}

QJsonObject DataIngestionServer::translateSenMLToV3(const QJsonArray& senmlData)
{
    // Default values
    QString deviceName = "_Face-recog";
    QString profileName = "face_recog";
    QJsonArray readings;
    
    for (const auto& itemRef : senmlData) {
        QJsonObject item = itemRef.toObject();
        
        deviceName = "_Face-recog";
        if (item.contains("bn")) {
            QString bn = item["bn"].toString();
            if (!bn.isEmpty() && bn != "face_recog") {
                deviceName = bn;
            }
        }
        
        if (item.contains("n")) {
            QString resource = item["n"].toString();
            if (resource == "DeviceName") continue;
            
            QJsonObject reading;
            reading["apiVersion"] = "v3";
            reading["resourceName"] = resource;
            reading["deviceName"] = deviceName;
            reading["profileName"] = profileName;
            reading["origin"] = QDateTime::currentDateTime().toMSecsSinceEpoch() * 1000000;
            
            QString valStr;
            QString type = "String";
            
            if (item.contains("v")) {
                double val = item["v"].toDouble();
                if (resource == "face_count") {
                    type = "Int64";
                    valStr = QString::number((long long)val);
                } else if (resource == "confidence" || resource == "cam_temp" || resource == "temperature") {
                    type = "Float32";
                    valStr = QString::number(val);
                } else {
                    type = "Float64";
                    valStr = QString::number(val);
                }
            } else if (item.contains("vs")) {
                valStr = item["vs"].toString();
            }
            
            reading["value"] = valStr;
            reading["valueType"] = type;
            
            readings.append(reading);
        }
    }
    
    QJsonObject event;
    event["apiVersion"] = "v3";
    event["deviceName"] = deviceName;
    event["profileName"] = profileName;
    event["origin"] = QDateTime::currentDateTime().toMSecsSinceEpoch() * 1000000;
    event["readings"] = readings;
    
    QJsonObject root;
    root["apiVersion"] = "v3";
    root["event"] = event;
    
    return root;
}

void DataIngestionServer::sendResponse(QTcpSocket* socket, int statusCode, const QByteArray& body)
{
    QByteArray response;
    response.append(QString("HTTP/1.1 %1 OK\r\n").arg(statusCode).toUtf8());
    response.append("Content-Type: application/json\r\n");
    response.append(QString("Content-Length: %1\r\n").arg(body.size()).toUtf8());
    response.append("\r\n");
    response.append(body);
    socket->write(response);
    // Do not close immediately, let the client (python script) close or use keep-alive? 
    // Python requests usually closes. Safe to just flush.
    socket->flush();
}

void DataIngestionServer::onForwardRequestFinished()
{
    // Handled in lambda
}
