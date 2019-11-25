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

	QByteArray _verb;
	QByteArray _path;
	QByteArray _token;

	bool _hdrDone = false;
	qint64 _len = 0;
	QByteArray _content;
	QByteArray _contentType;
};

class HttpServer : public QTcpServer
{
	Q_OBJECT

	Q_PROPERTY(QJsonObject data READ data WRITE setData NOTIFY dataChanged)

public:
	explicit HttpServer(QObject *parent = nullptr);
	explicit HttpServer(quint16 port, QObject *parent = nullptr);

	quint16 port() const;
	QUrl url(const QString &subPath) const;
	QString generateToken();

	void verifyRunning();
	QJsonObject data() const;

	QJsonValue obtainData(const QByteArrayList &path) const;
	void applyData(const QByteArray &verb, QByteArrayList path, const QJsonObject &data = {});

	void setData(QJsonObject data);
	void setDefaultData();
	void setAdvancedData();

signals:
	void dataChanged(QJsonObject data);

private slots:
	void connected();

private:
	friend class HttpConnection;

	QByteArray _token;
	QJsonObject _data;

	QJsonValue applyDataImpl(bool isPut, QByteArrayList path, QJsonValue cData, const QJsonObject &data);
};

#endif // HTTPSERVER_H
