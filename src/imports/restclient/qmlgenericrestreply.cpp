#include "qmlgenericrestreply.h"
#include <QDebug>
#include <QRegularExpression>

QtRestClient::QmlGenericRestReply::QmlGenericRestReply(QJsonSerializer *serializer, QJSEngine *engine, int returnType, int errorType, QtRestClient::RestReply *reply) :
	QObject(reply),
	_serializer(serializer),
	_engine(engine),
	_returnType(returnType),
	_errorType(errorType),
	_reply(reply)
{}

QtRestClient::RestReply *QtRestClient::QmlGenericRestReply::reply() const
{
	return _reply;
}

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
	if(!checkOk(completedHandler))
		return;

	_reply->onCompleted([completedHandler](int code){
		auto fn = completedHandler;
		fn.call({code});
	});
}

void QtRestClient::QmlGenericRestReply::addSucceededHandler(const QJSValue &succeededHandler)
{
	if(!checkOk(succeededHandler))
		return;

	QPointer<QJsonSerializer> serializer{_serializer};
	QPointer<QJSEngine> engine{_engine};
	QPointer<RestReply> reply{_reply};
	auto type = _returnType;
	if(isList(type)) {
		_reply->onSucceeded([succeededHandler, serializer, engine, reply, type](int code, const QJsonArray &arr) {
			if(!serializer || !engine)
				return;
			try {
				auto fn = succeededHandler;
				auto var = serializer->deserialize(arr, type, reply);
				fn.call({code, engine->toScriptValue(var)});
			} catch(std::exception &e) {
				qCritical() << e.what();
			}
		});
	} else {
		_reply->onSucceeded([succeededHandler, serializer, engine, reply, type](int code, const QJsonObject &obj) {
			if(!serializer || !engine)
				return;
			try {
				auto fn = succeededHandler;
				auto var = type == QMetaType::Void ? QVariant{} : serializer->deserialize(obj, type, reply);
				fn.call({code, engine->toScriptValue(var)});
			} catch(std::exception &e) {
				qCritical() << e.what();
			}
		});
	}
}

void QtRestClient::QmlGenericRestReply::addFailedHandler(const QJSValue &failedHandler)
{
	if(!checkOk(failedHandler))
		return;

	QPointer<QJsonSerializer> serializer{_serializer};
	QPointer<QJSEngine> engine{_engine};
	QPointer<RestReply> reply{_reply};
	auto type = _errorType;
	if(isList(type)) {
		_reply->onFailed([failedHandler, serializer, engine, reply, type](int code, const QJsonArray &arr) {
			if(!serializer || !engine)
				return;
			try {
				auto fn = failedHandler;
				auto var = serializer->deserialize(arr, type, reply);
				fn.call({code, engine->toScriptValue(var)});
			} catch(std::exception &e) {
				qCritical() << e.what();
			}
		});
	} else {
		_reply->onFailed([failedHandler, serializer, engine, reply, type](int code, const QJsonObject &obj) {
			if(!serializer || !engine)
				return;
			try {
				auto fn = failedHandler;
				auto var = type == QMetaType::Void ? QVariant{} : serializer->deserialize(obj, type, reply);
				fn.call({code, engine->toScriptValue(var)});
			} catch(std::exception &e) {
				qCritical() << e.what();
			}
		});
	}
}

void QtRestClient::QmlGenericRestReply::addErrorHandler(const QJSValue &errorHandler)
{
	if(!checkOk(errorHandler))
		return;

	_reply->onError([errorHandler](const QString &error, int code, RestReply::ErrorType type){
		auto fn = errorHandler;
		fn.call({error, code, type});
	});
}

bool QtRestClient::QmlGenericRestReply::checkOk(const QJSValue &fn) const
{
	if(!_reply) {
		qWarning() << "Cannot assign a handler to an invalid reply";
		return false;
	}

	if(!fn.isCallable()) {
		qWarning() << "Passed JS object is not a function!";
		return false;
	}

	return true;
}

bool QtRestClient::QmlGenericRestReply::isList(int type) const
{
	const static QRegularExpression regex {
		QStringLiteral(R"__(^QList<.*>$)__"),
		QRegularExpression::OptimizeOnFirstUsageOption |
		QRegularExpression::DontCaptureOption
	};

	if(regex.match(QString::fromUtf8(QMetaType::typeName(type)).trimmed()).hasMatch())
		return true;
	else
		return false;
}
