#include "objectbuilder.h"

#include <QDebug>
#include <QFileInfo>

ObjectBuilder::ObjectBuilder() {}

void ObjectBuilder::build(const QFileInfo &inFile)
{
	if(root["$type"].toString("object") == "object")
		generateApiObject(inFile.baseName());
	else
		generateApiGadget(inFile.baseName());
}

void ObjectBuilder::generateApiObject(const QString &name)
{
	header << "test3";
	source << "test4";
	qDebug() << "object:" << name;
}

void ObjectBuilder::generateApiGadget(const QString &name)
{
	header << "test1";
	source << "test2";
	qDebug() << "gadget:" << name;
}
