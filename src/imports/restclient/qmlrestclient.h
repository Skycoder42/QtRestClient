#ifndef QTRESTCLIENT_QMLRESTCLIENT_H
#define QTRESTCLIENT_QMLRESTCLIENT_H

#include <QtCore/QObject>
#include <QtRestClient/RestClient>

#include "qmlrestclass.h"

namespace QtRestClient {

class QmlRestClient : public RestClient
{
	Q_OBJECT

	Q_PROPERTY(QQmlListProperty<QtRestClient::QmlRestClass> classes READ classes)

	Q_CLASSINFO("DefaultProperty", "classes")

public:
	explicit QmlRestClient(QObject *parent = nullptr);

	QQmlListProperty<QmlRestClass> classes();

private:
	QList<QmlRestClass*> _childClasses;
};

}

#endif // QTRESTCLIENT_QMLRESTCLIENT_H
