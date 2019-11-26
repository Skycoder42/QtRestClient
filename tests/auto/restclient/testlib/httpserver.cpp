#include "httpserver.h"

#include <QtCore/QCborArray>
#include <QJsonDocument>
#include <QTcpSocket>
#include <QtTest>
#include <QtHttpServer>

#include <QtRestClient/private/requestbuilder_p.h>

namespace {

class HttpError : public std::exception
{
public:
	HttpError(QHttpServerResponse::StatusCode code = QHttpServerResponse::StatusCode::BadRequest) :
		  _code{code}
	{}
	HttpError(QByteArray message, QHttpServerResponse::StatusCode code = QHttpServerResponse::StatusCode::BadRequest) :
		  _code{code},
		  _message{std::move(message)}
	{}
	HttpError(const QString &message, QHttpServerResponse::StatusCode code = QHttpServerResponse::StatusCode::BadRequest) :
		  HttpError{message.toUtf8(), code}
	{}
	HttpError(const char *message, QHttpServerResponse::StatusCode code = QHttpServerResponse::StatusCode::BadRequest) :
		  HttpError{QByteArray{message}, code}
	{}

	int code() const {
		return static_cast<int>(_code);
	}

	const char *what() const noexcept override {
		if (_what.isEmpty())
			_what = "[" + QByteArray::number(static_cast<int>(_code)) + "]: " + (_message.isEmpty() ? "<No message>" : _message);
		return _what.constData();
	}

	QHttpServerResponse response() noexcept {
		return QHttpServerResponse {
			"text/plain",
			_message,
			_code
		};
	}

private:
	QHttpServerResponse::StatusCode _code;
	QByteArray _message;

	mutable QByteArray _what;
};

}

HttpServer::HttpServer(QObject *parent) :
	  HttpServer(0, parent)
{}

HttpServer::HttpServer(quint16 port, QObject *parent) :
	  QObject{parent},
	  _server{new QHttpServer{this}}
{
	_port = _server->listen(QHostAddress::LocalHost, port);
}

quint16 HttpServer::port() const
{
	return static_cast<quint16>(_port);
}

QUrl HttpServer::url() const
{
	QUrl url;
	url.setScheme(QStringLiteral("http"));
	url.setHost(QStringLiteral("localhost"));
	url.setPort(port());
	return url;
}

QUrl HttpServer::url(const QString &subPath) const
{
	auto rUrl = url();
	rUrl.setPath(subPath);
	return rUrl;
}

QString HttpServer::generateToken()
{
	_token = QUuid::createUuid().toRfc4122().toBase64();
	const auto strToken = QString::fromUtf8(_token);
	_token = "Bearer " + _token;
	return strToken;
}

bool HttpServer::setupRoutes()
{
	auto ok = false;
	[&]() {
		QVERIFY(_port > 0);
		QVERIFY(_server->route(QStringLiteral("/<arg>"), [this](const QString &type, const QHttpServerRequest &request) -> QHttpServerResponse {
			try {
				if (!_data.contains(type))
					throw HttpError{QHttpServerResponse::StatusCode::NotFound};
				const auto asJson = checkAccept(request);

				switch (request.method()) {
				case QHttpServerRequest::Method::Get: {
					const auto query = request.query();
					const auto offset = query.hasQueryItem(QStringLiteral("offset")) ?
																					 request.query().queryItemValue(QStringLiteral("offset")).toInt() :
																					 0;
					const auto limit = query.hasQueryItem(QStringLiteral("limit")) ?
																				   request.query().queryItemValue(QStringLiteral("limit")).toInt() :
																				   -1;

					QCborArray out;
					for (const auto elem : _data[type].toMap()) {
						const auto id = elem.first.toInteger();
						if (id >= offset && (limit == -1 || id < limit))
							out.append(elem.second);
					}

					return reply(asJson, out);
				}
				case QHttpServerRequest::Method::Post: {
					auto data = extract(request, true);
					auto tMap = _data[type].toMap();
					const auto tId = (tMap.end() - 1).key().toInteger() + 1;
					Q_ASSERT(!tMap.contains(tId));
					data[QStringLiteral("id")] = tId;
					tMap.insert(tId, data);
					_data[type] = tMap;
					return reply(asJson, data);
				}
				default:
					throw HttpError{QHttpServerResponse::StatusCode::MethodNotAllowed};
				}
			} catch (HttpError &e) {
				qWarning() << e.what();
				return e.response();
			}
		}));
		QVERIFY(_server->route(QStringLiteral("/<arg>/<arg>"), [this](const QString &type, int index, const QHttpServerRequest &request) -> QHttpServerResponse {
			try {
				if (!_data.contains(type))
					throw HttpError{QHttpServerResponse::StatusCode::NotFound};
				const auto asJson = checkAccept(request);
				auto tMap = _data[type].toMap();

				switch (request.method()) {
				case QHttpServerRequest::Method::Get:
					if (!tMap.contains(index))
						throw HttpError{QHttpServerResponse::StatusCode::NotFound};
					return reply(asJson, tMap[index]);
				case QHttpServerRequest::Method::Put: {
					auto data = extract(request, false);
					data[QStringLiteral("id")] = index;
					tMap.insert(index, data);
					_data[type] = tMap;
					return reply(asJson, data);
				}
				case QHttpServerRequest::Method::Delete:
					if (!tMap.contains(index))
						throw HttpError{QHttpServerResponse::StatusCode::NotFound};
					tMap.remove(index);
					_data[type] = tMap;
					return QHttpServerResponse::StatusCode::Ok;
				default:
					throw HttpError{QHttpServerResponse::StatusCode::MethodNotAllowed};
				}
			} catch (HttpError &e) {
				qWarning() << e.what();
				return e.response();
			}
		}));
		ok = true;
	}();
	return ok;
}

QCborMap HttpServer::data() const
{
	return _data;
}

void HttpServer::setData(QCborMap data)
{
	_data = std::move(data);
}

void HttpServer::setSubData(const QString &key, QCborMap data)
{
	_data[key] = std::move(data);
}

void HttpServer::setDefaultData()
{
	QCborMap root;

	QCborMap posts;
	for(auto i = 0; i < 100; i++) {
		posts[i] = QCborMap {
			{QStringLiteral("id"), i},
			{QStringLiteral("userId"), qCeil(i/2.0)},
			{QStringLiteral("title"), QStringLiteral("Title%1").arg(i)},
			{QStringLiteral("body"), QStringLiteral("Body%1").arg(i)}
		};
	}
	root[QStringLiteral("posts")] = posts;

	setData(root);
}

void HttpServer::setAdvancedData()
{
	QCborMap root;

	QCborMap posts;
	QCborMap postlets;
	for(auto i = 0; i < 100; i++) {
		//posts
		posts[i] = QCborMap {
			{QStringLiteral("id"), i},
			{QStringLiteral("userId"), qCeil(i/2.0)},
			{QStringLiteral("title"), QStringLiteral("Title%1").arg(i)},
			{QStringLiteral("body"), QStringLiteral("Body%1").arg(i)}
		};
		//postlets
		postlets[i] = QCborMap {
			{QStringLiteral("id"), i},
			{QStringLiteral("title"), QStringLiteral("Title%1").arg(i)},
			{QStringLiteral("href"), QStringLiteral("/posts/%1").arg(i)}
		};
	}
	root[QStringLiteral("posts")] = posts;
	root[QStringLiteral("postlets")] = postlets;

	QCborMap pages;
	QCborMap pagelets;
	for(auto i = 0; i < 10; i++) {
		//pages
		QCborMap page {
			{QStringLiteral("id"), i},
			{QStringLiteral("total"), 100},
			{QStringLiteral("offset"), i*10},
			{QStringLiteral("next"), i < 9 ?
				QStringLiteral("/pages/%1").arg(i + 1) :
				QCborValue{QCborValue::Null}
			},
			{QStringLiteral("previous"), i > 0 ?
				QStringLiteral("/pages/%1").arg(i - 1) :
				QCborValue{QCborValue::Null}
			},
		};

		QCborArray pageItems;
		for (auto j = 0; j < 10; j++)
			pageItems.append(posts[(i*10) + j]);
		page[QStringLiteral("items")] = pageItems;
		pages[i] = page;

		//pagelets
		QCborMap pagelet {
			{QStringLiteral("id"), i},
			{QStringLiteral("next"), i < 9 ?
				QStringLiteral("/pagelets/%1").arg(i + 1) :
				QCborValue{QCborValue::Null}
			},
		};

		QCborArray pageletItems;
		for (auto j = 0; j < 10; j++)
			pageletItems.append(postlets[(i*10) + j]);
		pagelet[QStringLiteral("items")] = pageletItems;
		pagelets[i] = pagelet;
	}
	root[QStringLiteral("pages")] = pages;
	root[QStringLiteral("pagelets")] = pagelets;

	setData(root);
}

bool HttpServer::checkAccept(const QHttpServerRequest &request)
{
	if (!_token.isEmpty()) {
		const auto cToken = request.headers().value(QStringLiteral("Authorization")).toByteArray();
		if (cToken != _token)
			throw HttpError{QHttpServerResponse::StatusCode::Unauthorized};
	}

	const auto accept = request.headers().value(QStringLiteral("Accept"), QtRestClient::RequestBuilderPrivate::ContentTypeJson);
	if (accept == QtRestClient::RequestBuilderPrivate::ContentTypeJson)
		return true;
	else if (accept == QtRestClient::RequestBuilderPrivate::ContentTypeCbor)
		return false;
	else
		throw HttpError{accept.toByteArray(), QHttpServerResponse::StatusCode::NotAcceptable};
}

QCborMap HttpServer::extract(const QHttpServerRequest &request, bool allowPost)
{
	const auto cType = request.headers()[QStringLiteral("Content-Type")].toByteArray();
	if (cType == QtRestClient::RequestBuilderPrivate::ContentTypeCbor) {
		QCborParserError error;
		const auto cbor = QCborValue::fromCbor(request.body(), &error);
		if (error.error != QCborError::NoError)
			throw HttpError{error.errorString()};
		if (cbor.type() == QCborValue::Map)
			throw HttpError{"Unexpected cbor type - must be a map"};
		return cbor.toMap();
	} else if (cType == QtRestClient::RequestBuilderPrivate::ContentTypeJson) {
		QJsonParseError error;
		const auto json = QJsonDocument::fromJson(request.body(), &error);
		if (error.error != QJsonParseError::NoError)
			throw HttpError{error.errorString()};
		if (!json.isObject())
			throw HttpError{"Unexpected json type - must be an object"};
		return QCborMap::fromJsonObject(json.object());
	} else if (allowPost && cType == QtRestClient::RequestBuilderPrivate::ContentTypeUrlEncoded) {
		QUrlQuery query {QString::fromUtf8(request.body())};
		QCborMap map;
		for(const auto &param : query.queryItems(QUrl::FullyDecoded))
			map.insert(param.first, param.second);
		return map;
	} else
		throw HttpError{cType, QHttpServerResponse::StatusCode::UnsupportedMediaType};
}

QHttpServerResponse HttpServer::reply(bool asJson, const QCborValue &value)
{
	if (asJson) {
		const auto jValue = value.toJsonValue();
		if (jValue.isObject())
			return jValue.toObject();
		else if (jValue.isArray())
			return jValue.toArray();
		else
			throw HttpError{QHttpServerResponse::StatusCode::InternalServerError};
	} else {
		return QHttpServerResponse {
			QtRestClient::RequestBuilderPrivate::ContentTypeCbor,
			QCborValue{value}.toCbor()
		};
	}
}
