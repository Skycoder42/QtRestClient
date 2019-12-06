#include "qmlrestreply.h"
#include <QtQml/QJSEngine>
#include <QtQml/QQmlInfo>
using namespace QtRestClient;

QmlRestReply::QmlRestReply(RestReply *reply, QJSEngine *engine, QObject *parent) :
	QObject{parent},
	_reply{reply},
	_engine{engine}
{}

void QmlRestReply::addCompletedHandler(const QJSValue &completedHandler)
{
	if (!checkOk(completedHandler))
		return;

	_reply->onCompleted([completedHandler](int code){
		auto fn = completedHandler;
		fn.call({code});
	});
}

void QmlRestReply::addSucceededHandler(const QJSValue &succeededHandler)
{
	if (!checkOk(succeededHandler))
		return;

	QPointer<QJSEngine> engine{_engine};
	QPointer<RestReply> reply{_reply};

	_reply->onSucceeded([this, succeededHandler, engine, reply](int code, const RestReply::DataType &data) {
		if (!engine)
			return;
		try {
			auto fn = succeededHandler;
			std::visit(__private::overload {
						   [&](std::nullopt_t) {
							   fn.call({code, QJSValue{}});
						   },
						   [&](const auto &vData) {
							   fn.call({code, engine->toScriptValue(vData.toVariant())});
						   }
					   }, data);
		} catch (std::exception &e) {
			qmlWarning(this) << "ERROR:" << e.what();
		}
	});
}

void QmlRestReply::addFailedHandler(const QJSValue &failedHandler)
{
	if (!checkOk(failedHandler))
		return;

	QPointer<QJSEngine> engine{_engine};
	QPointer<RestReply> reply{_reply};

	_reply->onFailed([this, failedHandler, engine, reply](int code, const RestReply::DataType &data) {
		if (!engine)
			return;
		try {
			auto fn = failedHandler;
			std::visit(__private::overload {
						   [&](std::nullopt_t) {
							   fn.call({code, QJSValue{}});
						   },
						   [&](const auto &vData) {
							   fn.call({code, engine->toScriptValue(vData.toVariant())});
						   }
					   }, data);
		} catch (std::exception &e) {
			qmlWarning(this) << "ERROR:" << e.what();
		}
	});
}

void QmlRestReply::addErrorHandler(const QJSValue &errorHandler)
{
	if (!checkOk(errorHandler))
		return;

	_reply->onError([errorHandler](const QString &error, int code, RestReply::Error type){
		auto fn = errorHandler;
		fn.call({error, code, static_cast<int>(type)});
	});
}

bool QmlRestReply::checkOk(const QJSValue &fn) const
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
