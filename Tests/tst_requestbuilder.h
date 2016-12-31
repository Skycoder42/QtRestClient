#ifndef TST_REQUESTBUILDER_H
#define TST_REQUESTBUILDER_H

#include <QNetworkAccessManager>
#include <QtTest>

class RequestBuilderTest : public QObject
{
	Q_OBJECT

public:
	RequestBuilderTest(QObject *parent);

private slots:
	void initTestCase();
	void cleanupTestCase();

	void testBuilding_data();
	void testBuilding();

	void testSending_data();
	void testSending();

private:
	QNetworkAccessManager *nam;
};

#endif // TST_REQUESTBUILDER_H
