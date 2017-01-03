#include "tst_global.h"

void initTestJsonServer(QString relativeDbPath)
{
	QDir targetDir(TEST_SRC_DIR);

	QFile inFile(targetDir.absoluteFilePath(relativeDbPath));
	if(!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
		QFAIL("Failed to open input file");
	QFile outFile(targetDir.absoluteFilePath("test-rest-db.json"));
	if(!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
		QFAIL("Failed to open input file");

	outFile.write(inFile.readAll());
	inFile.close();
	outFile.close();
	QThread::msleep(500);//Time for the server to reload the database
}
