#ifndef DATAINGESTIONSERVER_H
#define DATAINGESTIONSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkAccessManager>
#include <QNetworkReply>

struct RequestContext {
    QByteArray buffer;
    qint64 contentLength = -1;
    bool headersParsed = false;
};

class DataIngestionServer : public QObject
{
    Q_OBJECT
public:
    explicit DataIngestionServer(QObject *parent = nullptr);
    bool listen(quint16 port = 4000);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();
    void onForwardRequestFinished();

private:
    QTcpServer *m_server;
    QNetworkAccessManager *m_networkManager;
    QMap<QTcpSocket*, RequestContext> m_requests;
    
    void processRequest(QTcpSocket* socket, const QByteArray& data);
    void sendResponse(QTcpSocket* socket, int statusCode, const QByteArray& body);
    QJsonObject translateSenMLToV3(const QJsonArray& senmlData);
};

#endif // DATAINGESTIONSERVER_H
