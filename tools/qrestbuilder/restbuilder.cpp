#include "restbuilder.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

RestBuilder::RestBuilder() :
	QObject()
{}

void RestBuilder::build(const QString &in, const QString &hOut, const QString &cppOut)
{
	root = readJson(in);

	auto headerFile = new QFile(hOut, this);
	if(!headerFile->open(QIODevice::WriteOnly | QIODevice::Text))
		throwFile(*headerFile);
	header.setDevice(headerFile);

	auto sourceFile = new QFile(cppOut, this);
	if(!sourceFile->open(QIODevice::WriteOnly | QIODevice::Text))
		throwFile(*sourceFile);
	source.setDevice(sourceFile);

	build(in);

	header.flush();
	header.device()->close();
	source.flush();
	source.device()->close();
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

void RestBuilder::throwFile(const QFile &file)
{
	throw QString(file.fileName() + ": " + file.errorString());
}
