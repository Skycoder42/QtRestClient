#include "qmlrestclient.h"
using namespace QtRestClient;

QmlRestClient::QmlRestClient(QObject *parent) :
	RestClient(parent)
{}

QQmlListProperty<QmlRestClass> QmlRestClient::classes()
{
	return {this, _childClasses};
}
