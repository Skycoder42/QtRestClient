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

RestReply *QmlRestClass::call(const QString &verb, const QJSValue &arg0, const QJSValue &arg1, const QJSValue &arg2, const QJSValue &arg3, const QJSValue &arg4)
{
	return callImpl(verb.toUtf8(), arg0, arg1, arg2, arg3, arg4);
}

RestReply *QmlRestClass::get(const QJSValue &arg0, const QJSValue &arg1, const QJSValue &arg2)
{
	return callImpl(RestClass::GetVerb, arg0, arg1, arg2);
}

RestReply *QmlRestClass::post(const QJSValue &arg0, const QJSValue &arg1, const QJSValue &arg2, const QJSValue &arg3)
{
	return callImpl2(RestClass::PostVerb, true, arg0, arg1, arg2, arg3);
}

RestReply *QmlRestClass::put(const QJSValue &body, const QJSValue &arg0, const QJSValue &arg1, const QJSValue &arg2)
{
	return callImpl(RestClass::PutVerb, body, arg0, arg1, arg2);
}

RestReply *QmlRestClass::deleteResource(const QJSValue &arg0, const QJSValue &arg1, const QJSValue &arg2)
{
	return callImpl(RestClass::DeleteVerb, arg0, arg1, arg2);
}

RestReply *QmlRestClass::patch(const QJSValue &body, const QJSValue &arg0, const QJSValue &arg1, const QJSValue &arg2)
{
	return callImpl(RestClass::PatchVerb, body, arg0, arg1, arg2);
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

RestReply *QmlRestClass::callImpl(const QByteArray &verb, const QJSValue &arg0, const QJSValue &arg1, const QJSValue &arg2, const QJSValue &arg3, const QJSValue &arg4)
{
	return callImpl2(verb, false, arg0, arg1, arg2, arg3, arg4);
}

RestReply *QmlRestClass::callImpl2(const QByteArray &verb, bool forcePost, const QJSValue &arg0, const QJSValue &arg1, const QJSValue &arg2, const QJSValue &arg3, const QJSValue &arg4)
{
	if(!_class) {
		qmlWarning(this) << "ERROR: Cannot run call without a proper initialization";
		return nullptr;
	}

	QJsonValue body;
	QString path;
	QVariantHash params;
	auto asPostParams = forcePost;
	HeaderHash headers;
	auto offset = false;

	if(arg0.isArray() ||
	   arg0.isObject()) {
		body = QJsonValue::fromVariant(arg0.toVariant());
		offset = true;
	} else if(arg0.isUndefined())
		offset = true;

	const auto &b0 = offset ? arg1 : arg0;
	const auto &b1 = offset ? arg2 : arg1;
	const auto &b2 = offset ? arg3 : arg2;
	const auto &b3 = offset ? arg4 : arg3;
	offset = false;
	if(b0.isString()) {
		path = b0.toString();
		offset = true;
	}

	const auto &p0 = offset ? b1 : b0;
	const auto &p1 = offset ? b2 : b1;
	const auto &p2 = offset ? b3 : b2;
	offset = false;
	if(p0.isObject()) {
		params = p0.toVariant().toHash();
		if(p1.isBool()) {
			asPostParams = p1.toBool();
			offset = true;
		}

		const auto &m1 = offset ? p2 : p1;
		offset = false;
		if(m1.isObject()) {
			auto map = m1.toVariant().toMap();
			for(auto it = map.constBegin(); it != map.constEnd(); it++)
				headers.insert(it.key().toUtf8(), it.value().toString().toUtf8());
		}
	}

	if(body.isObject()) {
		if(!path.isEmpty())
			return _class->callJson(verb, path, body.toObject(), params, headers);
		else
			return _class->callJson(verb, body.toObject(), params, headers);
	} else if(body.isArray()) {
		if(!path.isEmpty())
			return _class->callJson(verb, path, body.toArray(), params, headers);
		else
			return _class->callJson(verb, body.toArray(), params, headers);
	} else if(!path.isEmpty())
		return _class->callJson(verb, path, params, headers, asPostParams);
	else
		return _class->callJson(verb, params, headers, asPostParams);
}
