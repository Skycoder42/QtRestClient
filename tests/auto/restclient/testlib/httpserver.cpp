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

QJsonValue HttpServer::obtainData(QByteArrayList path) const
{
	QJsonValue subValue = _data;

	foreach (auto segment, path) {
		if(segment.isEmpty())
			continue;

		if(subValue.isObject()) {
			auto subObj = subValue.toObject();
			if(!subObj.contains(QString::fromUtf8(segment)))
				throw QStringLiteral("path not found");
			else
				subValue = subObj.value(QString::fromUtf8(segment));
		} else if(subValue.isArray()) {
			auto subArray = subValue.toArray();
			auto ok = false;
			auto index = segment.toInt(&ok) - 1;//start counting at 1
			if(ok && index >= 0 && index < subArray.size())
				subValue = subArray.at(index);
			else
				throw QStringLiteral("path not found");
		} else
			throw QStringLiteral("path not found");
	}

	return subValue;
}

void HttpServer::applyData(const QByteArray &verb, QByteArrayList path, const QJsonObject &data)
{
	if(verb == "PUT")
		_data = applyDataImpl(true, path, _data, data).toObject();
	else if(verb == "POST")
		_data = applyDataImpl(false, path, _data, data).toObject();
	else if(verb == "DELETE")
		_data = applyDataImpl(true, path, _data, {}).toObject();
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

QJsonValue HttpServer::applyDataImpl(bool isPut, QByteArrayList path, QJsonValue cData, const QJsonObject &data)
{
	while(!path.isEmpty() && path.first().isEmpty())
		path.removeFirst();
	if(path.isEmpty())
		return data.isEmpty() ? QJsonValue(QJsonValue::Undefined) : data;

	auto segment = QString::fromUtf8(path.takeFirst());
	if(cData.isObject()) {
		auto obj = cData.toObject();
		auto newData = applyDataImpl(isPut, path, obj[segment], data);
		if(newData.isUndefined())
			obj.remove(segment);
		else
			obj[segment] = newData;
		return obj;
	} else if(cData.isArray()) {
		auto array = cData.toArray();

		auto ok = false;
		auto index = segment.toInt(&ok) - 1;//start counting at 1
		if(index < 0)
			throw QStringLiteral("invalid path index");
		while(array.size() <= index)
			array.append(QJsonValue::Null);

		auto newData = applyDataImpl(isPut, path, array[index], data);
		if(newData.isUndefined())
			array.removeAt(index);
		else
			array[index] = newData;
		return array;
	} else
		throw QStringLiteral("path not found");
}



HttpConnection::HttpConnection(QTcpSocket *socket, HttpServer *parent) :
	QObject(parent),
	_server(parent),
	_socket(socket),
	_verb(),
	_path(),
	_hdrDone(false),
	_len(0),
	_content()
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
		if(_hdrDone)
			reply();
		else {
			auto nextLine = _socket->readLine(bytes).simplified();
			if(nextLine.isEmpty()) {
				_hdrDone = true;
				reply();
			} else if(_verb.isEmpty()) {
				auto line = nextLine.split(' ');
				if(line.size() < 2)
					_socket->disconnectFromHost();
				else {
					_verb = line[0];
					_path = line[1];
				}
			} else if(nextLine.startsWith("Content-Length: "))
				_len = nextLine.mid(16).toInt();
		}
	}
}

void HttpConnection::reply()
{
	auto segments = _path.split('/');

	try {
		//read content if required
		if(_content.size() < _len) {
			_content += _socket->readAll();
			if(_len - _content.trimmed().size() > 0)
				return;
			_content = _content.trimmed();

			QJsonParseError e;
			auto obj = QJsonDocument::fromJson(_content, &e).object();
			if(e.error != QJsonParseError::NoError)
				throw QString(QStringLiteral("Parser-Error: ") + e.errorString());
			_server->applyData(_verb, segments, obj);
		}

		QJsonValue subValue = _server->obtainData(segments);
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
	} catch(QString &e) {
		qWarning().noquote() << "SERVER-Error[" << _verb <<  _path << "]:" << e;
		_socket->write("HTTP/1.1 404 NOT FOUND\r\n");
		_socket->write("Content-Length:0\r\n");
		_socket->write("Content-Type: application/json\r\n");
		_socket->write("Connection: Closed\r\n");
		_socket->write("\r\n");
	}

	_socket->flush();
}
