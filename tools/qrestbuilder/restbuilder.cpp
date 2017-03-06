#include "restbuilder.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

RestBuilder::RestBuilder(QObject *parent) :
	QObject(parent)
{}

void RestBuilder::buildClass(const QString &in, const QString &hOut, const QString &cppOut)
{
	auto root = readJson(in);
}

void RestBuilder::buildObject(const QString &in, const QString &hOut, const QString &cppOut)
{
	QFileInfo inInfo(in);
	auto root = readJson(in);

	QFile headerFile(hOut);
	if(!headerFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throwFile(headerFile);
	QTextStream headerStream(&headerFile);

	QFile sourceFile(cppOut);
	if(!sourceFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throwFile(sourceFile);
	QTextStream sourceStream(&sourceFile);

	if(root["$type"].toString("object") == "object")
		generateApiObject(inInfo.baseName(), root, headerStream, sourceStream);
	else
		generateApiGadget(inInfo.baseName(), root, headerStream, sourceStream);

	headerStream.flush();
	headerFile.close();
	sourceStream.flush();
	sourceFile.close();
}

QJsonObject RestBuilder::readJson(const QString &fileName)
{
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
		throwFile(file);

	QJsonParseError error;
	auto doc = QJsonDocument::fromJson(file.readAll(), &error);
	if(error.error != QJsonParseError::NoError)
		throw QString(fileName + ": " + error.errorString());
	file.close();

	return doc.object();
}

void RestBuilder::generateApiObject(const QString &name, const QJsonObject &obj, QTextStream &header, QTextStream &source)
{
	header << "test3";
	source << "test4";
	qDebug() << "object:" << name;
}

void RestBuilder::generateApiGadget(const QString &name, const QJsonObject &obj, QTextStream &header, QTextStream &source)
{
	header << "test1";
	source << "test2";
	qDebug() << "gadget:" << name;
}

void RestBuilder::throwFile(const QFile &file)
{
	throw QString(file.fileName() + ": " + file.errorString());
}
