#ifndef JPHUSER_H
#define JPHUSER_H

#include <QObject>
#include <QtRestClient>

struct JphUser
{
	Q_GADGET

	Q_PROPERTY(int id MEMBER id)
	Q_PROPERTY(QString name MEMBER name)

public:
	JphUser();

	int id;
	QString name;
};

struct JphUserSimple : public QtRestClient::Simple<JphUser>
{
	Q_GADGET

	Q_PROPERTY(int id MEMBER id)
	Q_PROPERTY(QUrl href MEMBER href)

public:
	JphUserSimple();

	QUrl extensionHref() const override;

	int id;
	QUrl href;
};

#endif // JPHUSER_H
