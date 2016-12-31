#ifndef TST_RESTCLIENT_H
#define TST_RESTCLIENT_H

#include <QObject>
#include <QtTest>

class RestClientTest : public QObject
{
	Q_OBJECT
public:
	explicit RestClientTest(QObject *parent);

private slots:
	void testbaseUrl_data();
	void testbaseUrl();
};

#endif // TST_RESTCLIENT_H
