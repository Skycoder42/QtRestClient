#include <QtCore>
#include <QtQuickTest/quicktest.h>
#include "testlib.h"

static void initImportPath()
{
#ifdef Q_OS_LINUX
	if(!qgetenv("LD_PRELOAD").contains("Qt5RestClient"))
		qWarning() << "No LD_PRELOAD set - this may fail on systems with multiple version of the modules";
#endif
	qputenv("QML2_IMPORT_PATH", QML_PATH);

	//start the http server
	auto server = new HttpServer(38723, qApp);
	server->verifyRunning();
	server->setAdvancedData();

	auto root = server->data();
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
	server->setData(root);
}
Q_COREAPP_STARTUP_FUNCTION(initImportPath)

QUICK_TEST_MAIN(qmlrestclient)
