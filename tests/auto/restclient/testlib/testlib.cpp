#include "testlib.h"
#include <QJSEngine>
#include <QJsonDocument>

void Testlib::initTestJsonServer(HttpServer *server, QString relativeDbPath)
{
	QDir targetDir(QStringLiteral(TESTLIB_SRC_DIR));
	targetDir.cdUp();

	QFile inFile(targetDir.absoluteFilePath(relativeDbPath));
	if(!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
		QFAIL("Failed to open input file");

	QJSEngine engine;
	auto result = engine.evaluate(QString::fromUtf8(inFile.readAll()));
	QVERIFY2(!result.isError(), qUtf8Printable(result.toString()));
	auto obj = QJsonValue::fromVariant(result.toVariant()).toObject();
	QVERIFY(!obj.isEmpty());

	inFile.close();

	server->setData(obj);
}

QtRestClient::RestClient *Testlib::createClient(QObject *parent)
{
	auto client = new QtRestClient::RestClient(parent);
	client->setModernAttributes();
	client->addRequestAttribute(QNetworkRequest::HTTP2AllowedAttribute, false);
	return client;
}
