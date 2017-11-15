#include "tst_global.h"
#include <QJSEngine>
#include <QJsonDocument>

void initTestJsonServer(QString relativeDbPath)
{
	QDir targetDir(TEST_SRC_DIR);

	QFile inFile(targetDir.absoluteFilePath(relativeDbPath));
	if(!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
		QFAIL("Failed to open input file");
	QFile outFile(targetDir.absoluteFilePath("test-rest-db.json"));
	if(!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
		QFAIL("Failed to open input file");

	if(QFileInfo(inFile).suffix() == "js") {
		QJSEngine engine;
		auto result = engine.evaluate(QString::fromUtf8(inFile.readAll()));
		QVERIFY2(!result.isError(), qUtf8Printable(result.toString()));
		auto doc = QJsonDocument::fromVariant(result.toVariant());
		QVERIFY(!doc.isNull());
		outFile.write(doc.toJson());
	} else
		outFile.write(inFile.readAll());

	inFile.close();
	outFile.close();
	QThread::sleep(10);//Time for the server to reload the database
}

QtRestClient::RestClient *createClient(QObject *parent)
{
	auto client = new QtRestClient::RestClient(parent);
	client->setModernAttributes();
	client->addRequestAttribute(QNetworkRequest::HTTP2AllowedAttribute, false);
	return client;
}
