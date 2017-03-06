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
	header << "\npublic:\n"
		   << "\tQ_INVOKABLE " << name << "(QObject *parent = nullptr);\n\n";
	writeReadDeclarations();
	header << "\npublic Q_SLOTS:\n";
	writeWriteDeclarations();
	header << "\nQ_SIGNALS:\n";
	writeNotifyDeclarations();
	header << "\nprivate:\n";
	writeMemberDefinitions(header);
	header << "};\n\n";

	//write source
	source << "#include \"" << name << ".h\"\n\n"
		   << name << "::" << name << "(QObject *parent) :\n"
		   << "\tQObject(parent)\n"
		   << "{}\n";
	writeReadDefinitions(name, false);
	writeWriteDefinitions(name, false);
}

void ObjectBuilder::generateApiGadget(const QString &name)
{
	qInfo() << "generating gadget:" << name;

	includes.insert("QSharedDataPointer", "QSharedDataPointer");
	parentType = {};
	readMembers();

	//write header
	writeIncludes(header, includes.values());
	header << "class " << name << "Data;\n";
	if(parentType.name.isEmpty())
		header << "class " << name << "\n";
	else
		header << "class " << name << " : public " << parentType.name << "\n";
	header << "{\n"
		   << "\tQ_GADGET\n\n";
	writeProperties(false);
	header << "\npublic:\n"
		   << "\t" << name << "();\n"
		   << "\t" << name << "(const " << name << " &other);\n"
		   << "\t~" << name << "();\n\n";
	writeReadDeclarations();
	header << "\n";
	writeWriteDeclarations();
	header << "\nprivate:\n"
		   << "\t QSharedDataPointer<" << name << "Data> d;\n"
		   << "};\n\n";

	//write source
	source << "#include \"" << name << ".h\"\n\n";
	writeDataClass(name + "Data");
	source << name << "::" << name << "() :\n"
		   << "\td(new " << name << "Data())\n"
		   << "{}\n\n"
		   << name << "::" << name << "(const " << name << " &other) :\n"
		   << "\td(other.d)\n"
		   << "{}\n\n"
		   << name << "::~" << name << "() {}\n";
	writeReadDefinitions(name, true);
	writeWriteDefinitions(name, true);
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

QString ObjectBuilder::setter(const QString &name)
{
	QString setterName = "set" + name;
	setterName[3] = setterName[3].toUpper();
	return setterName;
}

void ObjectBuilder::writeProperties(bool withNotify)
{
	for(auto it = members.constBegin(); it != members.constEnd(); it++) {
		header << "\tQ_PROPERTY(" << it->name << " " << it.key()
			   << " READ " << it.key()
			   << " WRITE " << setter(it.key());
		if(withNotify)
			header << " NOTIFY " << it.key() << "Changed";
		header << ")\n";
	}
}

void ObjectBuilder::writeReadDeclarations()
{
	for(auto it = members.constBegin(); it != members.constEnd(); it++)
		header << "\t" << it->name << " " << it.key() << "() const;\n";
}

void ObjectBuilder::writeWriteDeclarations()
{
	for(auto it = members.constBegin(); it != members.constEnd(); it++)
		header << "\tvoid " << setter(it.key()) << "(" << it->name << " " << it.key() << ");\n";
}

void ObjectBuilder::writeNotifyDeclarations()
{
	for(auto it = members.constBegin(); it != members.constEnd(); it++)
		header << "\tvoid " << it.key() << "Changed(" << it->name << " " << it.key() << ");\n";
}

void ObjectBuilder::writeMemberDefinitions(QTextStream &stream)
{
	for(auto it = members.constBegin(); it != members.constEnd(); it++)
		stream << "\t" << it->name << " _" << it.key() << ";\n";
}

void ObjectBuilder::writeReadDefinitions(const QString &className, bool asGadget)
{
	auto prefix = asGadget ? QStringLiteral("d->_") : QStringLiteral("_");
	for(auto it = members.constBegin(); it != members.constEnd(); it++) {
		source << "\n" << it->name << " " << className << "::" << it.key() << "() const\n"
			   << "{\n"
			   << "\treturn " << prefix << it.key() << ";\n"
			   << "}\n";
	}
}

void ObjectBuilder::writeWriteDefinitions(const QString &className, bool asGadget)
{
	auto prefix = asGadget ? QStringLiteral("d->_") : QStringLiteral("_");
	for(auto it = members.constBegin(); it != members.constEnd(); it++) {
		source << "\nvoid " << className << "::" << setter(it.key()) << "(" << it->name << " " << it.key() << ")\n"
			   << "{\n"
			   << "\tif(" << prefix << it.key() << " == " << it.key() << ")\n"
			   << "\t\treturn;\n\n"
			   << "\t" << prefix << it.key() << " = " << it.key() <<";\n";
		if(!asGadget)
			source << "\temit " << it.key() << "Changed(" << it.key() << ");\n";
		source << "}\n";
	}
}

void ObjectBuilder::writeDataClass(const QString &className)
{
	source << "class " << className << " : public QSharedData\n"
		   << "{\n"
		   << "public:\n"
		   << "\t" << className << "();\n"
		   << "\t" << className << "(const " << className << " &other);\n\n";
	writeMemberDefinitions(source);
	source << "};\n\n"
		   << className << "::" << className << "() :\n"
		   << "\tQSharedData()\n"
		   << "{}\n\n"
		   << className << "::" << className << "(const " << className << " &other) :\n"
		   << "\tQSharedData(other)\n";
	writeMemberCopyDefinitions(source);
	source << "{}\n\n";
}

void ObjectBuilder::writeMemberCopyDefinitions(QTextStream &stream)
{
	for(auto it = members.constBegin(); it != members.constEnd(); it++)
		stream << "\t,_" << it.key() << "(other._" << it.key() << ")\n";
}
