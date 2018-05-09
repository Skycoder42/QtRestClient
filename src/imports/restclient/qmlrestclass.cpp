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
	revaluateClass();
}

void QmlRestClass::setPath(QString path)
{
	if (_path == path)
		return;

	_path = std::move(path);
	emit pathChanged(_path);
}

void QmlRestClass::revaluateClass()
{
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
