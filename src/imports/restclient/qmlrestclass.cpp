#include "qmlrestclass.h"
#include <QQmlInfo>
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

RestReply *QmlRestClass::call(const QString &verb, const QVariant &postArg0, const QVariant &postArg1, const QVariant &postArg2, const QVariant &postArg3)
{
	QString path;
	QVariantHash params;
	HeaderHash headers;
	bool asPostParams;
	std::tie(path, params, headers, asPostParams) = extractParams(postArg0, postArg1, postArg2, postArg3);
	return callImpl(verb.toUtf8(), path, params, headers, asPostParams);
}

RestReply *QmlRestClass::call(const QVariantMap &body, const QString &verb, const QVariant &postArg0, const QVariant &postArg1, const QVariant &postArg2)
{
	QString path;
	QVariantHash params;
	HeaderHash headers;
	bool asPostParams;
	std::tie(path, params, headers, asPostParams) = extractParams(postArg0, postArg1, postArg2);
	return callImpl(verb.toUtf8(), path, QJsonObject::fromVariantMap(body), params, headers);
}

RestReply *QmlRestClass::call(const QVariantList &body, const QString &verb, const QVariant &postArg0, const QVariant &postArg1, const QVariant &postArg2)
{
	QString path;
	QVariantHash params;
	HeaderHash headers;
	bool asPostParams;
	std::tie(path, params, headers, asPostParams) = extractParams(postArg0, postArg1, postArg2);
	return callImpl(verb.toUtf8(), path, QJsonArray::fromVariantList(body), params, headers);
}

RestReply *QmlRestClass::get(const QVariant &postArg0, const QVariant &postArg1, const QVariant &postArg2)
{
	QString path;
	QVariantHash params;
	HeaderHash headers;
	bool asPostParams;
	std::tie(path, params, headers, asPostParams) = extractParams(postArg0, postArg1, postArg2);
	return callImpl(RestClass::GetVerb, path, params, headers);
}

RestReply *QmlRestClass::post(const QVariant &postArg0, const QVariant &postArg1, const QVariant &postArg2)
{
	QString path;
	QVariantHash params;
	HeaderHash headers;
	bool asPostParams;
	std::tie(path, params, headers, asPostParams) = extractParams(postArg0, postArg1, postArg2);
	return callImpl(RestClass::PostVerb, path, params, headers, true);
}

RestReply *QmlRestClass::post(const QVariantMap &body, const QVariant &postArg0, const QVariant &postArg1, const QVariant &postArg2)
{
	QString path;
	QVariantHash params;
	HeaderHash headers;
	bool asPostParams;
	std::tie(path, params, headers, asPostParams) = extractParams(postArg0, postArg1, postArg2);
	return callImpl(RestClass::PostVerb, path, QJsonObject::fromVariantMap(body), params, headers);
}

RestReply *QmlRestClass::post(const QVariantList &body, const QVariant &postArg0, const QVariant &postArg1, const QVariant &postArg2)
{
	QString path;
	QVariantHash params;
	HeaderHash headers;
	bool asPostParams;
	std::tie(path, params, headers, asPostParams) = extractParams(postArg0, postArg1, postArg2);
	return callImpl(RestClass::PostVerb, path, QJsonArray::fromVariantList(body), params, headers);
}

RestReply *QmlRestClass::put(const QVariantMap &body, const QVariant &postArg0, const QVariant &postArg1, const QVariant &postArg2)
{
	QString path;
	QVariantHash params;
	HeaderHash headers;
	bool asPostParams;
	std::tie(path, params, headers, asPostParams) = extractParams(postArg0, postArg1, postArg2);
	return callImpl(RestClass::PutVerb, path, QJsonObject::fromVariantMap(body), params, headers);
}

RestReply *QmlRestClass::put(const QVariantList &body, const QVariant &postArg0, const QVariant &postArg1, const QVariant &postArg2)
{
	QString path;
	QVariantHash params;
	HeaderHash headers;
	bool asPostParams;
	std::tie(path, params, headers, asPostParams) = extractParams(postArg0, postArg1, postArg2);
	return callImpl(RestClass::PutVerb, path, QJsonArray::fromVariantList(body), params, headers);
}

RestReply *QmlRestClass::deleteResource(const QVariant &postArg0, const QVariant &postArg1, const QVariant &postArg2)
{
	QString path;
	QVariantHash params;
	HeaderHash headers;
	bool asPostParams;
	std::tie(path, params, headers, asPostParams) = extractParams(postArg0, postArg1, postArg2);
	return callImpl(RestClass::DeleteVerb, path, params, headers);
}

RestReply *QmlRestClass::patch(const QVariantMap &body, const QVariant &postArg0, const QVariant &postArg1, const QVariant &postArg2)
{
	QString path;
	QVariantHash params;
	HeaderHash headers;
	bool asPostParams;
	std::tie(path, params, headers, asPostParams) = extractParams(postArg0, postArg1, postArg2);
	return callImpl(RestClass::PatchVerb, path, QJsonObject::fromVariantMap(body), params, headers);
}

RestReply *QmlRestClass::patch(const QVariantList &body, const QVariant &postArg0, const QVariant &postArg1, const QVariant &postArg2)
{
	QString path;
	QVariantHash params;
	HeaderHash headers;
	bool asPostParams;
	std::tie(path, params, headers, asPostParams) = extractParams(postArg0, postArg1, postArg2);
	return callImpl(RestClass::PatchVerb, path, QJsonArray::fromVariantList(body), params, headers);
}

void QmlRestClass::setPath(QString path)
{
	if (_path == path)
		return;

	_path = std::move(path);
	emit pathChanged(_path);
	revaluateClass();
}

void QmlRestClass::revaluateClass()
{
	if(!_init)
		return;
	if(_class) {
		emit restClassChanged(nullptr);
		_class->deleteLater();
	}
	_class = nullptr;

	auto client = qobject_cast<RestClient*>(parent());
	if(client)
		_class = client->createClass(_path, this);
	else {
		RestClass *pClass = nullptr;
		auto qpClass = qobject_cast<QmlRestClass*>(parent());
		if(qpClass)
			pClass = qpClass->restClass();
		else
			pClass = qobject_cast<RestClass*>(parent());

		if(pClass) {
			_class = pClass->subClass(_path, this);
			disconnect(qpClass, &QmlRestClass::restClassChanged,
					   this, &QmlRestClass::revaluateClass);
		} else if(qpClass) {
			connect(qpClass, &QmlRestClass::restClassChanged,
					this, &QmlRestClass::revaluateClass,
					Qt::UniqueConnection);
		} else
			qmlWarning(this) << "ERROR: A RestClass must be a child element of a RestClient or another RestClass! Further use of this instance will lead to crashes!";
	}

	if(_class)
		emit restClassChanged(_class);
}

std::tuple<QString, QVariantHash, HeaderHash, bool> QmlRestClass::extractParams(const QVariant &arg0, const QVariant &arg1, const QVariant &arg2, const QVariant &arg3)
{
	QString path;
	QVariantHash params;
	bool asPostParams = false;
	HeaderHash headers;
	auto offset = false;

	if(arg0.type() == QVariant::String) {
		path = arg0.toString();
		offset = true;
	}

	const auto &p0 = offset ? arg1 : arg0;
	const auto &p1 = offset ? arg2 : arg1;
	const auto &p2 = offset ? arg3 : arg2;
	offset = false;
	if(p0.type() == QVariant::Map) {
		params = p0.toHash();
		if(p1.type() == QVariant::Bool) {
			asPostParams = p1.toBool();
			offset = true;
		}

		const auto &m1 = offset ? p2 : p1;
		offset = false;
		if(m1.type() == QVariant::Map) {
			auto map = m1.toMap();
			for(auto it = map.constBegin(); it != map.constEnd(); it++)
				headers.insert(it.key().toUtf8(), it.value().toString().toUtf8());
		}
	}

	return std::make_tuple(path, params, headers, asPostParams);
}

template<typename... Args>
RestReply *QmlRestClass::callImpl(const QByteArray &verb, const QString &path, Args... args)
{
	if(!_class) {
		qmlWarning(this) << "ERROR: Cannot run call without a proper initialization";
		return nullptr;
	}

	QUrl pUrl(path);
	if(pUrl.isValid())
		return _class->callJson(verb, pUrl, args...);
	else if(!path.isEmpty())
		return _class->callJson(verb, path, args...);
	else
		return _class->callJson(verb, args...);
}
