#include "objectbuilder.h"

#include <QDebug>
#include <QFileInfo>

ObjectBuilder::ObjectBuilder() {}

void ObjectBuilder::build()
{
	if(root["$type"].toString("object") == "object")
		generateApiObject(inFile.baseName());
	else
		generateApiGadget(inFile.baseName());
}

void ObjectBuilder::generateApiObject(const QString &name)
{
	qInfo() << "generating object:" << name;

	includes.insert(QLatin1String("QObject"), QLatin1String("QObject"));
	readMembers();

	writeIncGuardBegin();
	writeInclude(header, includes.values());
	writeIncGuardEnd();
}

void ObjectBuilder::generateApiGadget(const QString &name)
{
	qInfo() << "generating gadget:" << name;

	includes.insert(QLatin1String("QSharedDataPointer"), QLatin1String("QSharedDataPointer"));
	readMembers();

	writeIncGuardBegin();
	writeInclude(header, includes.values());
	writeIncGuardEnd();
}

void ObjectBuilder::readMembers()
{
	for(auto it = root.constBegin(); it != root.constEnd(); it++) {
		auto type = splitType(it.value().toString());
		if(!type.second.isEmpty())
			includes.insert(type.first, type.second);
		members.insert(it.key(), type.first);
	}
}
