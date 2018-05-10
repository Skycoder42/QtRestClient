#ifndef QMLPAGING_H
#define QMLPAGING_H

#include <QtCore/QObject>
#include <QtQml/QJSValue>
#include <QtRestClient/IPaging>
#include <QtRestClient/RestReply>
#include <QtRestClient/RestClient>
#include <QJSEngine>

namespace QtRestClient {

class QmlPaging
{
	Q_GADGET

	Q_PROPERTY(QVariantList items READ items CONSTANT)
	Q_PROPERTY(int total READ total CONSTANT)
	Q_PROPERTY(int offset READ offset CONSTANT)
	Q_PROPERTY(bool hasNext READ hasNext CONSTANT)
	Q_PROPERTY(QUrl nextUrl READ nextUrl CONSTANT)
	Q_PROPERTY(bool hasPrevious READ hasPrevious CONSTANT)
	Q_PROPERTY(QUrl previousUrl READ previousUrl CONSTANT)
	Q_PROPERTY(QVariantMap properties READ properties CONSTANT)

public:
	QmlPaging(IPaging *iPaging, RestClient *client, QJSEngine *engine);

	static QmlPaging create(RestClient *client, QJSEngine *engine, const QJsonObject &obj);

	Q_INVOKABLE QtRestClient::RestReply *next();
	Q_INVOKABLE QtRestClient::RestReply *previous();

	QVariantList items() const;
	int total() const;
	int offset() const;
	bool hasNext() const;
	QUrl nextUrl() const;
	bool hasPrevious() const;
	QUrl previousUrl() const;
	QVariantMap properties() const;

public Q_SLOTS:
	void iterate(const QJSValue &iterator,
				 int to = -1, int from = 0);
	void iterate(const QJSValue &iterator,
				 const QJSValue &failureHandler,
				 const QJSValue &errorHandler,
				 int to = -1, int from = 0);

private:
	QJSEngine *_engine;
	RestClient *_client;
	QSharedPointer<IPaging> _paging;

	int internalIterate(QJSValue iterator, int from, int to) const;
};

}

#endif // QMLPAGING_H
