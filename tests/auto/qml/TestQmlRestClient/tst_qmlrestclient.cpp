#include <QtCore>
#include <QQmlEngine>
#include <QQmlContext>
#include <QtQuickTest/quicktest.h>
#include "testlib.h"

namespace {

QPointer<HttpServer> _server;

}

namespace TestSvr {

quint16 getPort() {
	return _server->serverPort();
}

}

class Setup : public QObject
{
	Q_OBJECT

public slots:
	void qmlEngineAvailable(QQmlEngine *engine)
	{
		engine->rootContext()->setContextProperty("testPort", _server->serverPort());
	}
};

static void initImportPath()
{
	//start the http server
	_server = new HttpServer(qApp);
	_server->verifyRunning();
	_server->setAdvancedData();

	auto root = _server->data();
	QJsonObject vRoot;
	QJsonArray posts;
	for(auto i = 0; i < 100; i++) {
		posts.append(QJsonObject {
						 {QStringLiteral("id"), i},
						 {QStringLiteral("user"), QJsonObject {
							  {QStringLiteral("id"), qCeil(i/2.0)},
							  {QStringLiteral("name"), QStringLiteral("user%1").arg(qCeil(i/2.0))},
						  }},
						 {QStringLiteral("title"), QStringLiteral("Title%1").arg(i)},
						 {QStringLiteral("body"), QStringLiteral("Body%1").arg(i)}
					 });
	}
	vRoot[QStringLiteral("posts")] = posts;
	root[QStringLiteral("v1")] = vRoot;
	_server->setData(root);
}
Q_COREAPP_STARTUP_FUNCTION(initImportPath)

QUICK_TEST_MAIN_WITH_SETUP(qmlrestclient, Setup)

#include "tst_qmlrestclient.moc"
