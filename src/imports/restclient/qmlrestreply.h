#ifndef QTRESTCLIENT_QMLRESTREPLY_H
#define QTRESTCLIENT_QMLRESTREPLY_H

#include <QtCore/QPointer>

#include <QtRestClient/RestReply>

#include <QtQml/QJSValue>
#include <QtQml/QJSEngine>

namespace QtRestClient {

class QmlRestReply : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QtRestClient::RestReply *reply MEMBER _reply CONSTANT)

public:
	explicit QmlRestReply(RestReply *reply,
						  QJSEngine *engine,
						  QObject *parent = nullptr);

public Q_SLOTS:
	void addCompletedHandler(const QJSValue &completedHandler);
	void addSucceededHandler(const QJSValue &succeededHandler);
	void addFailedHandler(const QJSValue &failedHandler);
	void addErrorHandler(const QJSValue &errorHandler);

private:
	QPointer<RestReply> _reply;
	QJSEngine *_engine;

	bool checkOk(const QJSValue &fn) const;
};

}

#endif // QMLRESTREPLY_H
