#include "qmlgenericrestreply.h"
#include <QtQml/QQmlInfo>
using namespace QtJsonSerializer;

QtRestClient::QmlGenericRestReply::QmlGenericRestReply(SerializerBase *serializer, QJSEngine *engine, int returnType, int errorType, QtRestClient::RestReply *reply) :
	QObject{reply},
	_serializer{serializer},
	_engine{engine},
	_returnType{returnType},
	_errorType{errorType},
	_reply{reply}
{}

QString QtRestClient::QmlGenericRestReply::returnType() const
{
	return QString::fromUtf8(QMetaType::typeName(_returnType));
}

QString QtRestClient::QmlGenericRestReply::errorType() const
{
	return QString::fromUtf8(QMetaType::typeName(_errorType));
}

void QtRestClient::QmlGenericRestReply::addCompletedHandler(const QJSValue &completedHandler)
{
	if (!checkOk(completedHandler))
		return;

	_reply->onCompleted([completedHandler](int code){
		auto fn = completedHandler;
		fn.call({code});
	});
}

void QtRestClient::QmlGenericRestReply::addSucceededHandler(const QJSValue &succeededHandler)
{
	if (!checkOk(succeededHandler))
		return;

	QPointer<SerializerBase> serializer{_serializer};
	QPointer<QJSEngine> engine{_engine};
	QPointer<RestReply> reply{_reply};
	auto type = _returnType;

	_reply->onSucceeded([this, succeededHandler, serializer, engine, reply, type](int code, const RestReply::DataType &data) {
		if (!serializer || !engine)
			return;
		try {
			auto fn = succeededHandler;
			std::visit(__private::overload {
						   [&](std::nullopt_t) {
							   fn.call({code, QJSValue{}});
						   },
						   [&](const auto &vData) {
							   const auto var = serializer->deserializeGeneric(vData, type, reply);
							   fn.call({code, engine->toScriptValue(var)});
						   }
			}, data);
		} catch (std::exception &e) {
			qmlWarning(this) << "ERROR:" << e.what();
		}
	});
}

void QtRestClient::QmlGenericRestReply::addFailedHandler(const QJSValue &failedHandler)
{
	if (!checkOk(failedHandler))
		return;

	QPointer<SerializerBase> serializer{_serializer};
	QPointer<QJSEngine> engine{_engine};
	QPointer<RestReply> reply{_reply};
	auto type = _errorType;
	_reply->onFailed([this, failedHandler, serializer, engine, reply, type](int code, const RestReply::DataType &data) {
		if(!serializer || !engine)
			return;
		try {
			auto fn = failedHandler;
			std::visit(__private::overload {
						   [&](std::nullopt_t) {
							   fn.call({code, QJSValue{}});
						   },
						   [&](const auto &vData) {
							   try {
								   const auto var = serializer->deserializeGeneric(vData, type, reply);
								   fn.call({code, engine->toScriptValue(var)});
							   } catch (DeserializationException &e) {
								   fn.call({code, QString::fromUtf8(e.message())});
								   throw;
							   }
						   }
					   }, data);
		} catch (std::exception &e) {
			qmlWarning(this) << "ERROR:" << e.what();
		}
	});
}

void QtRestClient::QmlGenericRestReply::addErrorHandler(const QJSValue &errorHandler)
{
	if (!checkOk(errorHandler))
		return;

	_reply->onError([errorHandler](const QString &error, int code, RestReply::Error type){
		auto fn = errorHandler;
		fn.call({error, code, static_cast<int>(type)});
	});
}

bool QtRestClient::QmlGenericRestReply::checkOk(const QJSValue &fn) const
{
	if (!_reply) {
		qmlWarning(this) << "Cannot assign a handler to an invalid reply";
		return false;
	}

	if (!fn.isCallable()) {
		qmlWarning(this) << "Passed JS object is not a callable function!";
		return false;
	}

	return true;
}
