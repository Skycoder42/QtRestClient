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
	if(root["$type"].toString("object") == "object")
		generateApiObject(inInfo.baseName(), root);
	else
		generateApiGadget(inInfo.baseName(), root);
}

QJsonObject RestBuilder::readJson(const QString &fileName)
{
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
		throw file.errorString();

	QJsonParseError error;
	auto doc = QJsonDocument::fromJson(file.readAll(), &error);
	if(error.error != QJsonParseError::NoError)
		throw error.errorString();
	file.close();

	return doc.object();
}

void RestBuilder::generateApiObject(const QString &name, const QJsonObject &obj)
{
	qDebug() << "object:" << name;
}

void RestBuilder::generateApiGadget(const QString &name, const QJsonObject &obj)
{
	qDebug() << "gadget:" << name;
}
