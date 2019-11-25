#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QtCore/QCborValue>
#include <QtCore/QCborMap>
#include <QtCore/QJsonObject>

#include <QtHttpServer/QHttpServer>

class HttpServer : public QObject
{
	Q_OBJECT

public:
	explicit HttpServer(QObject *parent = nullptr);
	explicit HttpServer(quint16 port, QObject *parent = nullptr);

	quint16 port() const;
	QUrl url() const;
	QUrl url(const QString &subPath) const;
	QString generateToken();

	bool setupRoutes();

	QCborMap data() const;
	void setData(QCborMap data);
	void setSubData(const QString &key, QCborMap data);
	void setDefaultData();
	void setAdvancedData();

private:
	QHttpServer *_server;
	int _port = -1;
	QByteArray _token;
	QCborMap _data;

	bool checkAccept(const QHttpServerRequest &request);
	QCborMap extract(const QHttpServerRequest &request, bool allowPost);
	QHttpServerResponse reply(bool asJson, const QCborValue &value);
};

#endif // HTTPSERVER_H
