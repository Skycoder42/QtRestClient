#ifndef QTRESTCLIENT_QMLRESTCLASS_H
#define QTRESTCLIENT_QMLRESTCLASS_H

#include <tuple>
#include <QtCore/QObject>
#include <QtQml/QQmlParserStatus>
#include <QtQml/QQmlListProperty>
#include <QtQml/QJSValue>

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

	// [body,] [path,] [params, [asPost], [headers]]
	Q_INVOKABLE QtRestClient::RestReply *call(const QString &verb,
											  const QJSValue &optBody = {},
											  const QJSValue &optPath = {},
											  const QJSValue &optParams = {},
											  const QJSValue &optAsPost = {},
											  const QJSValue &optHeaders = {});

	// [path,] [params, [headers]]
	Q_INVOKABLE QtRestClient::RestReply *get(const QJSValue &optPath = {}, const QJSValue &optParams = {}, const QJSValue &optHeaders = {});

	// [body,] [path,] [params, [headers]]
	Q_INVOKABLE QtRestClient::RestReply *post(const QJSValue &optBody = {}, const QJSValue &optPath = {}, const QJSValue &optParams = {}, const QJSValue &optHeaders = {});

	// body, [path,] [params, [headers]]
	Q_INVOKABLE QtRestClient::RestReply *put(const QJSValue &body, const QJSValue &optPath = {}, const QJSValue &optParams = {}, const QJSValue &optHeaders = {});

	// [path,] [params, [headers]]
	Q_INVOKABLE QtRestClient::RestReply *deleteResource(const QJSValue &optPath = {}, const QJSValue &optParams = {}, const QJSValue &optHeaders = {});

	// body, [path,] [params, [headers]]
	Q_INVOKABLE QtRestClient::RestReply *patch(const QJSValue &body, const QJSValue &optPath = {}, const QJSValue &optParams = {}, const QJSValue &optHeaders = {});

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
	bool _init = false;

	// [body,] [path,] [params, [asPost], [headers]]
	RestReply *callImpl(const QByteArray &verb, const QJSValue &arg0 = {}, const QJSValue &arg1 = {}, const QJSValue &arg2 = {}, const QJSValue &arg3 = {}, const QJSValue &arg4 = {});
	RestReply *callImpl2(const QByteArray &verb, bool forcePost, const QJSValue &arg0 = {}, const QJSValue &arg1 = {}, const QJSValue &arg2 = {}, const QJSValue &arg3 = {}, const QJSValue &arg4 = {});
};

}

#endif // QTRESTCLIENT_QMLRESTCLASS_H
