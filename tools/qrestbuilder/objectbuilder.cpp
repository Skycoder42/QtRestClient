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

	includes.insert("QObject", "QObject");
	parentType = {QStringLiteral("QObject"), false, QStringLiteral("QObject")};
	readMembers();

	//write header
	writeIncludes(header, includes.values());
	header << "class " << name << " : public " << parentType.name << "\n"
		   << "{\n"
		   << "\tQ_OBJECT\n\n";
	writeProperties(true);
	header << "public:\n"
		   << "\tQ_INVOKABLE " << name << "(QObject *parent = nullptr);\n"
		   << "};\n\n";

	source << "#include \"" << name << ".h\"\n\n"
		   << name << "::" << name << "(QObject *parent) :\n"
		   << "\tQObject(parent)\n"
		   << "{}\n\n";
}

void ObjectBuilder::generateApiGadget(const QString &name)
{
	qInfo() << "generating gadget:" << name;

	includes.insert("QSharedDataPointer", "QSharedDataPointer");
	parentType = {};
	readMembers();

	writeIncludes(header, includes.values());
}

void ObjectBuilder::readMembers()
{
	if(root.contains("$parent"))
		parentType = readType(root["$parent"].toString());
	if(!parentType.include.isEmpty())
		includes.insert(parentType.name, parentType.include);

	for(auto it = root.constBegin(); it != root.constEnd(); it++) {
		if(it.key().startsWith("$"))
			continue;
		auto type = readType(it.value().toString());
		if(!type.include.isEmpty())
			includes.insert(type.name, type.include);
		members.insert(it.key(), type);
	}
}

void ObjectBuilder::writeProperties(bool withNotify)
{
	for(auto it = members.constBegin(); it != members.constEnd(); it++) {
		QString setterName = "set" + it.key();
		setterName[3] = setterName[3].toUpper();
		header << "\tQ_PROPERTY(" << it->name << " " << it.key()
			   << " READ " << it.key()
			   << " WRITE " << setterName;
		if(withNotify)
			header << " NOTIFY " << it.key() << "Changed";
		header << ")\n";
	}

	header << '\n';
}
