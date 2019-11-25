#include <QtCore>
#include <QQmlEngine>
#include <QQmlContext>
#include <QtQuickTest/quicktest.h>
#include "testlib.h"

namespace {

QPointer<HttpServer> _server;

}

namespace TestSvr {

QUrl getUrl() {
	return _server->url();
}

}

class Setup : public QObject
{
	Q_OBJECT

public slots:
	void qmlEngineAvailable(QQmlEngine *engine)
	{
		engine->rootContext()->setContextProperty("testPort", _server->port());
	}
};

static void initImportPath()
{
	//start the http server
	_server = new HttpServer(qApp);
	QVERIFY(_server->setupRoutes());
	_server->setAdvancedData();

	QCborMap vRoot;
	QCborMap posts;
	for(auto i = 0; i < 100; i++) {
		posts[i] = QCborMap {
			{QStringLiteral("id"), i},
			{QStringLiteral("user"), QCborMap {
				{QStringLiteral("id"), qCeil(i/2.0)},
				{QStringLiteral("name"), QStringLiteral("user%1").arg(qCeil(i/2.0))},
			}},
			{QStringLiteral("title"), QStringLiteral("Title%1").arg(i)},
			{QStringLiteral("body"), QStringLiteral("Body%1").arg(i)}
		};
	}
	vRoot[QStringLiteral("posts")] = posts;
	_server->setSubData(QStringLiteral("v1"), vRoot);
}
Q_COREAPP_STARTUP_FUNCTION(initImportPath)

QUICK_TEST_MAIN_WITH_SETUP(qmlrestclient, Setup)

#include "tst_qmlrestclient.moc"
