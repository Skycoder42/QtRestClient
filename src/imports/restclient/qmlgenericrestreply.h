#ifndef QTRESTCLIENT_QMLGENERICRESTREPLY_H
#define QTRESTCLIENT_QMLGENERICRESTREPLY_H

#include <QtCore/QObject>
#include <QtQml/QJSValue>
#include <QtQml/QJSEngine>
#include <QtJsonSerializer/QJsonSerializer>
#include <QtRestClient/RestReply>

namespace QtRestClient {

class QmlGenericRestReply : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QtRestClient::RestReply *reply READ reply CONSTANT)
	Q_PROPERTY(QString returnType READ returnType CONSTANT)
	Q_PROPERTY(QString errorType READ errorType CONSTANT)

public:
	Q_INVOKABLE QmlGenericRestReply(QJsonSerializer *serializer,
									QJSEngine *engine,
									int returnType,
									int errorType,
									QtRestClient::RestReply *reply);

	QtRestClient::RestReply * reply() const;
	QString returnType() const;
	QString errorType() const;

public Q_SLOTS:
	void addCompletedHandler(const QJSValue &completedHandler);
	void addSucceededHandler(const QJSValue &succeededHandler);
	void addFailedHandler(const QJSValue &failedHandler);
	void addErrorHandler(const QJSValue &errorHandler);

private:
	QJsonSerializer *_serializer;
	QJSEngine *_engine;
	int _returnType;
	int _errorType;
	RestReply* _reply;

	bool checkOk(const QJSValue &fn) const;
	bool isList(int type) const;
};

}

Q_DECLARE_METATYPE(QtRestClient::QmlGenericRestReply*)

#endif // QTRESTCLIENT_QMLGENERICRESTREPLY_H
