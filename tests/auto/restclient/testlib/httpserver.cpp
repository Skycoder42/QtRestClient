#include "httpserver.h"

#include <QJsonDocument>
#include <QTcpSocket>
#include <QtTest>

HttpServer::HttpServer(QObject *parent) :
	HttpServer(0, parent)
{}

HttpServer::HttpServer(quint16 port, QObject *parent) :
	QTcpServer(parent)
{
	connect(this, &HttpServer::newConnection,
			this, &HttpServer::connected);

	listen(QHostAddress::LocalHost, port);
}

QUrl HttpServer::url(const QString &subPath) const
{
	return QUrl(QStringLiteral("http://localhost:%1/%2")
				.arg(serverPort())
				.arg(subPath));
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
			auto index = segment.toInt(&ok);
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

void HttpServer::setDefaultData()
{
	QJsonObject root;

	QJsonArray posts;
	for(auto i = 0; i < 100; i++) {
		posts.append(QJsonObject {
						 {QStringLiteral("id"), i},
						 {QStringLiteral("userId"), qCeil(i/2.0)},
						 {QStringLiteral("title"), QStringLiteral("Title%1").arg(i)},
						 {QStringLiteral("body"), QStringLiteral("Body%1").arg(i)}
					 });
	}
	root[QStringLiteral("posts")] = posts;

	setData(root);
}

void HttpServer::setAdvancedData()
{
	QJsonObject root;

	QJsonArray posts;
	QJsonArray pages;
	QJsonArray postlets;
	QJsonArray pagelets;

	for(auto i = 0; i < 100; i++) {
		//posts
		posts.append(QJsonObject {
						 {QStringLiteral("id"), i},
						 {QStringLiteral("userId"), qCeil(i/2.0)},
						 {QStringLiteral("title"), QStringLiteral("Title%1").arg(i)},
						 {QStringLiteral("body"), QStringLiteral("Body%1").arg(i)}
					 });
		//postlets
		postlets.append(QJsonObject {
						 {QStringLiteral("id"), i},
						 {QStringLiteral("title"), QStringLiteral("Title%1").arg(i)},
						 {QStringLiteral("href"), QStringLiteral("/posts/%1").arg(i)}
					 });
	}
	root[QStringLiteral("posts")] = posts;
	root[QStringLiteral("postlets")] = postlets;

	for(auto i = 0; i < 10; i++) {
		//pages
		QJsonObject page {
			{QStringLiteral("id"), i},
			{QStringLiteral("total"), 100},
			{QStringLiteral("offset"), i*10},
			{QStringLiteral("next"), i < 9 ?
								QStringLiteral("/pages/%1").arg(i + 1) :
								QJsonValue(QJsonValue::Null)},
			{QStringLiteral("previous"), i > 0 ?
								QStringLiteral("/pages/%1").arg(i - 1) :
								QJsonValue(QJsonValue::Null)},
		};

		QJsonArray pageItems;
		for(auto j = 0; j < 10; j++)
			pageItems.append(posts[(i*10) + j]);
		page[QStringLiteral("items")] = pageItems;
		pages.append(page);

		//pagelets
		QJsonObject pagelet {
			{QStringLiteral("id"), i},
			{QStringLiteral("next"), i < 9 ?
								QStringLiteral("/pagelets/%1").arg(i + 1) :
								QJsonValue(QJsonValue::Null)},
		};

		QJsonArray pageletItems;
		for(auto j = 0; j < 10; j++)
			pageletItems.append(postlets[(i*10) + j]);
		pagelet[QStringLiteral("items")] = pageletItems;
		pagelets.append(pagelet);
	}
	root[QStringLiteral("pages")] = pages;
	root[QStringLiteral("pagelets")] = pagelets;

	setData(root);
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
		auto index = segment.toInt(&ok);
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
	auto superPath = _path.split('?');
	auto segments = superPath.first().split('/');

	QByteArray doc;
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
		if(subValue.isObject())
			doc = QJsonDocument(subValue.toObject()).toJson(QJsonDocument::Compact);
		else
			doc = QJsonDocument(subValue.toArray()).toJson(QJsonDocument::Compact);

		_socket->write("HTTP/1.1 200 OK\r\n");
	} catch(QString &e) {
		qWarning().noquote() << "SERVER-Error[" << _verb <<  _path << "]:" << e;

		QJsonObject error;
		error[QStringLiteral("message")] = e;
		doc = QJsonDocument(error).toJson(QJsonDocument::Compact);

		_socket->write("HTTP/1.1 404 Not Found\r\n");
	}

	_socket->write("Content-Length: " + QByteArray::number(doc.size()) + "\r\n");
	_socket->write("Content-Type: application/json\r\n");
	_socket->write("Connection: Closed\r\n");
	_socket->write("\r\n");
	_socket->write(doc + "\r\n");
	_socket->flush();
}
