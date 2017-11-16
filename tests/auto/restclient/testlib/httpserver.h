#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QJsonObject>
#include <QTcpServer>

class HttpServer;
class HttpConnection : public QObject
{
	Q_OBJECT

public:
	HttpConnection(QTcpSocket *socket, HttpServer *parent = nullptr);

private slots:
	void readyRead();
	void reply();

private:
	HttpServer *_server;
	QTcpSocket *_socket;
	QByteArray verb;
	QByteArray path;
};

class HttpServer : public QTcpServer
{
	Q_OBJECT

	Q_PROPERTY(QJsonObject data READ data WRITE setData NOTIFY dataChanged)

public:
	explicit HttpServer(QObject *parent = nullptr);

	void verifyRunning();
	QJsonObject data() const;

public slots:
	void setData(QJsonObject data);

signals:
	void dataChanged(QJsonObject data);

private slots:
	void connected();

private:
	QJsonObject _data;
};

#endif // HTTPSERVER_H
