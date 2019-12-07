#include "qmlrestclass.h"
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
using namespace QtRestClient;

QmlRestClass::QmlRestClass(QObject *parent) :
	QObject(parent)
{}

RestClass *QmlRestClass::restClass() const
{
	return _class;
}

QString QmlRestClass::path() const
{
	return _path;
}

QQmlListProperty<QmlRestClass> QmlRestClass::classes()
{
	return {this, _childClasses};
}

void QmlRestClass::classBegin() {}

void QmlRestClass::componentComplete()
{
	_init = true;
	revaluateClass();
}

QmlRestReply *QmlRestClass::call(const QString &verb, QJSValue optPath, QJSValue optBody, QJSValue optParams, QJSValue optAsPost, QJSValue optHeaders)
{
	const auto path = getPath(optPath, optBody, optParams, optAsPost, optHeaders);
	const auto body = getBody(optBody, optParams, optAsPost, optHeaders);
	const auto params = getParams(optParams, optAsPost, optHeaders);
	const auto asPost = getAsPost(body.has_value(), optAsPost, optHeaders);
	const auto headers = getHeaders(optHeaders);
	return callImpl(verb.toUtf8(), path, body, params, asPost, headers);
}

QmlRestReply *QmlRestClass::get(QJSValue optPath, QJSValue optParams, QJSValue optHeaders)
{
	const auto path = getPath(optPath, optParams, optHeaders);
	const auto params = getParams(optParams, optHeaders);
	const auto headers = getHeaders(optHeaders);
	return callImpl(RestClass::GetVerb, path, std::nullopt, params, false, headers);
}

QmlRestReply *QmlRestClass::post(QJSValue optPath, QJSValue optBodyParams, QJSValue optParams, QJSValue optHeaders)
{
	const auto path = getPath(optPath, optBodyParams, optParams, optHeaders);
	std::optional<std::variant<QCborValue, QJsonValue>> body;
	std::optional<QVariantHash> params;
	bool postParams;
	if (optParams.isUndefined() ||
		(optParams.isObject() && optHeaders.isObject())) {
		// parameters are either explicitly unset, or all arguments are used -> body must be a body
		body = getBody(optBodyParams, optParams, optHeaders);
		params = getParams(optParams, optHeaders);
		postParams = false;
	} else if (optBodyParams.isObject()) {
		// body is an object -> use as post params
		params = getParams(optBodyParams, optParams, optHeaders);
		// optParams must be the headers now -> shift over (headers must be undefined)
		shift(optParams, optHeaders);
		postParams = true;
	} else {
		// body is an array or anything -> process as body
		body = getBody(optBodyParams, optParams, optHeaders);
		params = getParams(optParams, optHeaders);
		postParams = false;
	}
	const auto headers = getHeaders(optHeaders);
	return callImpl(RestClass::PostVerb, path, body, params, postParams, headers);
}

QmlRestReply *QmlRestClass::put(QJSValue optPathOrBody, QJSValue body, QJSValue optParams, QJSValue optHeaders)
{
	const auto path = getPath(optPathOrBody, body, optParams, optHeaders);
	const auto rBody = getBody(body, optParams, optHeaders);
	if (!rBody) {
		qmlWarning(this) << "ERROR: no body specified on put request";
		return nullptr;
	}
	const auto params = getParams(optParams, optHeaders);
	const auto headers = getHeaders(optHeaders);
	return callImpl(RestClass::PutVerb, path, rBody, params, false, headers);
}

QmlRestReply *QmlRestClass::deleteResource(QJSValue optPath, QJSValue optParams, QJSValue optHeaders)
{
	const auto path = getPath(optPath, optParams, optHeaders);
	const auto params = getParams(optParams, optHeaders);
	const auto headers = getHeaders(optHeaders);
	return callImpl(RestClass::DeleteVerb, path, std::nullopt, params, false, headers);
}

QmlRestReply *QmlRestClass::patch(QJSValue optPathOrBody, QJSValue body, QJSValue optParams, QJSValue optHeaders)
{
	const auto path = getPath(optPathOrBody, body, optParams, optHeaders);
	const auto rBody = getBody(body, optParams, optHeaders);
	if (!rBody) {
		qmlWarning(this) << "ERROR: no body specified on patch request";
		return nullptr;
	}
	const auto params = getParams(optParams, optHeaders);
	const auto headers = getHeaders(optHeaders);
	return callImpl(RestClass::PatchVerb, path, rBody, params, false, headers);
}

QmlRestReply *QmlRestClass::head(QJSValue optPath, QJSValue optParams, QJSValue optHeaders)
{
	const auto path = getPath(optPath, optParams, optHeaders);
	const auto params = getParams(optParams, optHeaders);
	const auto headers = getHeaders(optHeaders);
	return callImpl(RestClass::HeadVerb, path, std::nullopt, params, false, headers);
}

void QmlRestClass::setPath(QString path)
{
	if (_path == path)
		return;

	_path = std::move(path);
	Q_EMIT pathChanged(_path);
	revaluateClass();
}

void QmlRestClass::revaluateClass()
{
	if (!_init)
		return;
	if (_class) {
		Q_EMIT restClassChanged(nullptr);
		_class->deleteLater();
	}
	_class = nullptr;

	auto client = qobject_cast<RestClient*>(parent());
	if (client)
		_class = client->createClass(_path, this);
	else {
		RestClass *pClass = nullptr;
		auto qpClass = qobject_cast<QmlRestClass*>(parent());
		if (qpClass)
			pClass = qpClass->restClass();
		else
			pClass = qobject_cast<RestClass*>(parent());

		if (pClass) {
			_class = pClass->subClass(_path, this);
			disconnect(qpClass, &QmlRestClass::restClassChanged,
					   this, &QmlRestClass::revaluateClass);
		} else if (qpClass) {
			connect(qpClass, &QmlRestClass::restClassChanged,
					this, &QmlRestClass::revaluateClass,
					Qt::UniqueConnection);
		} else
			qmlWarning(this) << "ERROR: A RestClass must be a child element of a RestClient or another RestClass! Further use of this instance will lead to crashes!";
	}

	if (_class)
		Q_EMIT restClassChanged(_class);
}

template<typename... TValues>
bool QmlRestClass::shift(QJSValue &first, QJSValue &second, TValues&... values) const
{
	if (!shift(second, values...))
		return false;
	second = std::move(first);
	return true;
}

bool QmlRestClass::shift(QJSValue &end) const
{
	return end.isUndefined();
}

template<typename... TValues>
std::optional<std::variant<QString, QUrl>> QmlRestClass::getPath(QJSValue &optPath, TValues&... values) const
{
	if (optPath.isString())
		return optPath.toString();
	else if (const auto variant = optPath.toVariant(); variant.userType() == QMetaType::QUrl)
		return variant.toUrl();
	else if (optPath.isUndefined())
		return std::nullopt;
	else {
		if (shift(optPath, values...))
			optPath = {};
		else
			qmlWarning(this) << "Unsupported parameter configuration: Argument is not a path, but cannot be shifted either";
		return std::nullopt;
	}
}

template<typename... TValues>
std::optional<std::variant<QCborValue, QJsonValue>> QmlRestClass::getBody(QJSValue &optBody, TValues&... values) const
{
	if (optBody.isArray() || optBody.isObject()) {
		switch (_class->client()->dataMode()) {
		case RestClient::DataMode::Cbor:
			return QCborValue::fromVariant(optBody.toVariant());
		case RestClient::DataMode::Json:
			return QJsonValue::fromVariant(optBody.toVariant());
		default:
			Q_UNREACHABLE();
		}
	} else if (optBody.isUndefined())
		return std::nullopt;
	else {
		if (shift(optBody, values...))
			optBody = {};
		else
			qmlWarning(this) << "Unsupported parameter configuration: Argument is not an object or array, but cannot be shifted either";
		return std::nullopt;
	}
}

template<typename... TValues>
std::optional<QVariantHash> QmlRestClass::getParams(QJSValue &optParams, TValues&... values) const
{
	if (optParams.isObject())
		return optParams.toVariant().toHash();
	else if (optParams.isUndefined())
		return std::nullopt;
	else {
		if (shift(optParams, values...))
			optParams = {};
		else
			qmlWarning(this) << "Unsupported parameter configuration: Argument is not a paremeter object, but cannot be shifted either";
		return std::nullopt;
	}
}

template<typename... TValues>
bool QmlRestClass::getAsPost(bool hasBody, QJSValue &optAsPost, TValues&... values) const
{
	if (optAsPost.isBool())
		return hasBody ? false : optAsPost.toBool();
	else if (optAsPost.isUndefined())
		return false;
	else {
		if (shift(optAsPost, values...))
			optAsPost = {};
		else
			qmlWarning(this) << "Unsupported parameter configuration: Argument is not a boolean, but cannot be shifted either";
		return false;
	}
}

std::optional<HeaderHash> QmlRestClass::getHeaders(QJSValue &optHeaders) const
{
	if (optHeaders.isObject()) {
		const auto headerMap = optHeaders.toVariant().toMap();
		HeaderHash headers;
		headers.reserve(headerMap.size());
		for (auto it = headerMap.constBegin(); it != headerMap.constEnd(); it++)
			headers.insert(it.key().toUtf8(), it.value().toString().toUtf8());
		return headers;
	} else if (optHeaders.isUndefined())
		return std::nullopt;
	else {
		qmlWarning(this) << "Unsupported parameter configuration: Argument is not a header object, but is the last argument";
		return std::nullopt;
	}
}

QmlRestReply *QmlRestClass::makeQmlReply(RestReply *reply)
{
	auto qmlReply = new QmlRestReply{reply, QQmlEngine::contextForObject(this)->engine(), nullptr};
	QQmlEngine::setObjectOwnership(qmlReply, QQmlEngine::JavaScriptOwnership);
	return qmlReply;
}

QmlRestReply *QmlRestClass::callImpl(const QByteArray &verb, const std::optional<std::variant<QString, QUrl> > &optPath, const std::optional<std::variant<QCborValue, QJsonValue> > &optBody, const std::optional<QVariantHash> &optParams, bool optAsPost, const std::optional<HeaderHash> &optHeaders)
{
	if (optPath) {
		return std::visit([&](const auto &path) {
			if (optBody) {
				return std::visit([&](const auto &body) {
					return makeQmlReply(_class->callRaw(verb, path, body, optParams.value_or(QVariantHash{}), optHeaders.value_or(HeaderHash{})));
				}, *optBody);
			} else
				return makeQmlReply(_class->callRaw(verb, path, optParams.value_or(QVariantHash{}), optHeaders.value_or(HeaderHash{}), optAsPost));
		}, *optPath);
	} else {
		if (optBody) {
			return std::visit([&](const auto &body) {
				return makeQmlReply(_class->callRaw(verb, body, optParams.value_or(QVariantHash{}), optHeaders.value_or(HeaderHash{})));
			}, *optBody);
		} else
			return makeQmlReply(_class->callRaw(verb, optParams.value_or(QVariantHash{}), optHeaders.value_or(HeaderHash{}), optAsPost));
	}
}
