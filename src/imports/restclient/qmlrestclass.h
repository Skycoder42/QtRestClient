#ifndef QTRESTCLIENT_QMLRESTCLASS_H
#define QTRESTCLIENT_QMLRESTCLASS_H

#include <tuple>
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

	Q_INVOKABLE QtRestClient::RestReply *call(const QString &verb, const QVariant &postArg0 = {}, const QVariant &postArg1 = {}, const QVariant &postArg2 = {}, const QVariant &postArg3 = {});
	Q_INVOKABLE QtRestClient::RestReply *call(const QVariantMap &body, const QString &verb, const QVariant &postArg0 = {}, const QVariant &postArg1 = {}, const QVariant &postArg2 = {});
	Q_INVOKABLE QtRestClient::RestReply *call(const QVariantList &body, const QString &verb, const QVariant &postArg0 = {}, const QVariant &postArg1 = {}, const QVariant &postArg2 = {});

	Q_INVOKABLE QtRestClient::RestReply *get(const QVariant &postArg0 = {}, const QVariant &postArg1 = {}, const QVariant &postArg2 = {});

	Q_INVOKABLE QtRestClient::RestReply *post(const QVariant &postArg0 = {}, const QVariant &postArg1 = {}, const QVariant &postArg2 = {});
	Q_INVOKABLE QtRestClient::RestReply *post(const QVariantMap &body, const QVariant &postArg0 = {}, const QVariant &postArg1 = {}, const QVariant &postArg2 = {});
	Q_INVOKABLE QtRestClient::RestReply *post(const QVariantList &body, const QVariant &postArg0 = {}, const QVariant &postArg1 = {}, const QVariant &postArg2 = {});

	Q_INVOKABLE QtRestClient::RestReply *put(const QVariantMap &body, const QVariant &postArg0 = {}, const QVariant &postArg1 = {}, const QVariant &postArg2 = {});
	Q_INVOKABLE QtRestClient::RestReply *put(const QVariantList &body, const QVariant &postArg0 = {}, const QVariant &postArg1 = {}, const QVariant &postArg2 = {});

	Q_INVOKABLE QtRestClient::RestReply *deleteResource(const QVariant &postArg0 = {}, const QVariant &postArg1 = {}, const QVariant &postArg2 = {});

	Q_INVOKABLE QtRestClient::RestReply *patch(const QVariantMap &body, const QVariant &postArg0 = {}, const QVariant &postArg1 = {}, const QVariant &postArg2 = {});
	Q_INVOKABLE QtRestClient::RestReply *patch(const QVariantList &body, const QVariant &postArg0 = {}, const QVariant &postArg1 = {}, const QVariant &postArg2 = {});

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

	// [path], [params, [asPost], [headers]]
	std::tuple<QString, QVariantHash, HeaderHash, bool> extractParams(const QVariant &arg0, const QVariant &arg1, const QVariant &arg2, const QVariant &arg3 = {});
	template <typename... Args>
	RestReply *callImpl(const QByteArray &verb, const QString &path, Args... args);
};

}

#endif // QTRESTCLIENT_QMLRESTCLASS_H
