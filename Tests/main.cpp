#include <QtTest>

#include "tst_requestbuilder.h"
#include "tst_restclient.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	app.setAttribute(Qt::AA_Use96Dpi, true);
	QTEST_SET_MAIN_SOURCE_PATH

	QList<QObject*> tests = {
		new RequestBuilderTest(qApp),
		new RestClientTest(qApp)
	};

	auto res = 0;
	foreach(auto test, tests)
		res += QTest::qExec(test, argc, argv);
	return res;
}
