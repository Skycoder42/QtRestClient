#ifndef SIMPLEJPHPOST_H
#define SIMPLEJPHPOST_H

#include <QObject>
#include <QtRestClient>
#include <jphpost.h>

class SimpleJphPost : public QtRestClient::Simple<JphPost*>
{
	Q_OBJECT

	Q_PROPERTY(int id MEMBER id)
	Q_PROPERTY(QString title MEMBER title)
	Q_PROPERTY(QUrl href MEMBER href)

public:
	Q_INVOKABLE SimpleJphPost(QObject *parent = nullptr);
	SimpleJphPost(int id, QString title, QUrl href, QObject *parent = nullptr);

	QTRESTCLIENT_EXT_HREF_PROP(href)

	int id;
	QString title;
	QUrl href;
};

#endif // SIMPLEJPHPOST_H
