#ifndef TST_REQUESTBUILDER_H
#define TST_REQUESTBUILDER_H

#include <QtTest>

class RequestBuilderTest : public QObject
{
	Q_OBJECT

public:
	RequestBuilderTest(QObject *parent);

private slots:
	void testBuilding_data();
	void testBuilding();
};

#endif // TST_REQUESTBUILDER_H
