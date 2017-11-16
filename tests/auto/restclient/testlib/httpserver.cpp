#include "httpserver.h"

#include <QJsonDocument>
#include <QTcpSocket>
#include <QtTest>

HttpServer::HttpServer(QObject *parent) :
	QTcpServer(parent)
{
	connect(this, &HttpServer::newConnection,
			this, &HttpServer::connected);

	listen(QHostAddress::LocalHost, 43653);//DEBUG switch port
}

void HttpServer::verifyRunning()
{
	QVERIFY(isListening());
	QCOMPARE(serverError(), QAbstractSocket::UnknownSocketError);
}

QJsonObject HttpServer::data() const
{
	return _data;
}

void HttpServer::setData(QJsonObject data)
{
	if (_data == data)
		return;

	_data = data;
	emit dataChanged(_data);
}

void HttpServer::connected()
{
	while(hasPendingConnections())
		new HttpConnection(nextPendingConnection(), this);
}



HttpConnection::HttpConnection(QTcpSocket *socket, HttpServer *parent) :
	QObject(parent),
	_server(parent),
	_socket(socket)
{
	_socket->setParent(this);

	connect(socket, &QTcpSocket::readyRead,
			this, &HttpConnection::readyRead);
	connect(socket, &QTcpSocket::disconnected,
			this, &HttpConnection::deleteLater);
}

void HttpConnection::readyRead()
{
	qint64 bytes = 0;
	while((bytes = _socket->bytesAvailable()) >= 2) {
		auto nextLine = _socket->readLine(bytes).simplified();
		if(nextLine.isEmpty())
			reply();
		else if(verb.isEmpty()) {
			auto line = nextLine.split(' ');
			if(line.size() < 2)
				_socket->disconnectFromHost();
			else {
				verb = line[0];
				path = line[1];
			}
		}
	}
}

void HttpConnection::reply()
{
	auto segments = path.split('/');

	QJsonValue subValue = _server->data();
	auto error = false;
	foreach (auto segment, segments) {
		if(segment.isEmpty())
			continue;

		if(subValue.isObject()) {
			auto subObj = subValue.toObject();
			if(!subObj.contains(QString::fromUtf8(segment))) {
				error = true;
				break;
			} else
				subValue = subObj.value(QString::fromUtf8(segment));
		} else if(subValue.isArray()) {
			auto subArray = subValue.toArray();
			auto ok = false;
			auto index = segment.toInt(&ok) - 1;//start counting at 1
			if(ok && index >= 0 && index < subArray.size())
				subValue = subArray.at(index);
			else
				error = true;
		} else
			error = true;
	}

	if(error) {
		_socket->write("HTTP/1.1 404 NOT FOUND\r\n");
		_socket->write("Content-Length:0\r\n");
		_socket->write("Content-Type: application/json\r\n");
		_socket->write("Connection: Closed\r\n");
		_socket->write("\r\n");
	} else {
		QByteArray doc;
		if(subValue.isObject())
			doc = QJsonDocument(subValue.toObject()).toJson(QJsonDocument::Compact);
		else
			doc = QJsonDocument(subValue.toArray()).toJson(QJsonDocument::Compact);

		_socket->write("HTTP/1.1 200 OK\r\n");
		_socket->write("Content-Length: " + QByteArray::number(doc.size()) + "\r\n");
		_socket->write("Content-Type: application/json\r\n");
		_socket->write("Connection: Closed\r\n");
		_socket->write("\r\n");
		_socket->write(doc + "\r\n");
	}
	_socket->flush();
}
