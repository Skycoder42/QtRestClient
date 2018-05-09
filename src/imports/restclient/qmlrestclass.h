#ifndef QTRESTCLIENT_QMLRESTCLASS_H
#define QTRESTCLIENT_QMLRESTCLASS_H

#include <QtCore/QObject>
#include <QtQml/QQmlParserStatus>
#include <QtQml/QQmlListProperty>

#include <QtRestClient/RestClass>

namespace QtRestClient {

class QmlRestClass : public QObject, public QQmlParserStatus
{
	Q_OBJECT
	Q_INTERFACES(QQmlParserStatus)

	Q_PROPERTY(QtRestClient::RestClass* restClass READ restClass NOTIFY restClassChanged)
	Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
	Q_PROPERTY(QQmlListProperty<QtRestClient::QmlRestClass> classes READ classes)

	Q_CLASSINFO("DefaultProperty", "classes")

public:
	explicit QmlRestClass(QObject *parent = nullptr);

	RestClass* restClass() const;
	QString path() const;
	QQmlListProperty<QmlRestClass> classes();

	void classBegin() override;
	void componentComplete() override;

public Q_SLOTS:
	void setPath(QString path);

Q_SIGNALS:
	void restClassChanged(QtRestClient::RestClass *restClass);
	void pathChanged(QString path);

private Q_SLOTS:
	void revaluateClass();

private:
	RestClass *_class = nullptr;
	QString _path;
	QList<QmlRestClass*> _childClasses;
};

}

#endif // QTRESTCLIENT_QMLRESTCLASS_H
