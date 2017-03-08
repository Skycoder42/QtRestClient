#include "objectbuilder.h"

#include <QDebug>
#include <QFileInfo>
#include <QJsonArray>

ObjectBuilder::ObjectBuilder() {}

void ObjectBuilder::build()
{
	if(root["$type"].toString() == "object")
		generateApiObject();
	else if(root["$type"].toString() == "gadget")
		generateApiGadget();
	else
		throw QStringLiteral("REST_API_OBJECTS must be either of type object or gadget");
}

QString ObjectBuilder::specialPrefix()
{
	return "$";
}

void ObjectBuilder::generateApiObject()
{
	qInfo() << "generating object:" << className;

	auto includes = readIncludes();
	includes.append("QtCore/qobject.h");
	includes.append("QtCore/qstring.h");
	readMembers();
	auto parent = root["$parent"].toString("QObject");

	//write header
	writeIncludes(header, includes);
	header << "class " << className << " : public " << parent << "\n"
		   << "{\n"
		   << "\tQ_OBJECT\n\n";
	writeProperties(true);
	header << "\npublic:\n";
	if(root.contains("$enums"))
		writeEnums();
	header << "\tQ_INVOKABLE " << className << "(QObject *parent = nullptr);\n\n";
	writeReadDeclarations();
	header << "\npublic Q_SLOTS:\n";
	writeWriteDeclarations();
	header << "\nQ_SIGNALS:\n";
	writeNotifyDeclarations();
	header << "\nprivate:\n";
	writeMemberDeclarations(header);
	header << "};\n\n";

	//write source
	writeSourceIncludes();
	source << className << "::" << className << "(QObject *parent) :\n"
		   << "\t" << parent << "(parent)\n";
	writeMemberDefinitions(source);
	source << "{}\n";
	writeReadDefinitions(false);
	writeWriteDefinitions(false);
	if(root["$registerConverters"].toBool(true))
		writeListConverter(true);
}

void ObjectBuilder::generateApiGadget()
{
	qInfo() << "generating gadget:" << className;

	auto includes = readIncludes();
	includes.append("QtCore/qobject.h");
	includes.append("QtCore/qshareddata.h");
	includes.append("QtCore/qstring.h");
	readMembers();
	auto parent = root["$parent"].toString();

	//write header
	writeIncludes(header, includes);
	header << "class " << className << "Data;\n";
	if(parent.isEmpty())
		header << "class " << className << "\n";
	else
		header << "class " << className << " : public " << parent << "\n";
	header << "{\n"
		   << "\tQ_GADGET\n\n";
	writeProperties(false);
	header << "\npublic:\n";
	if(root.contains("$enums"))
		writeEnums();
	header << "\t" << className << "();\n"
		   << "\t" << className << "(const " << className << " &other);\n"
		   << "\t~" << className << "();\n\n"
		   << "\t" << className << " &operator =(const " << className << " &other);\n\n";
	writeReadDeclarations();
	header << "\n";
	writeWriteDeclarations();
	header << "\nprivate:\n"
		   << "\t QSharedDataPointer<" << className << "Data> d;\n"
		   << "};\n\n";

	//write source
	writeSourceIncludes();
	writeDataClass();
	source << className << "::" << className << "() :\n";
	if(!parent.isEmpty())
		source << "\t" << parent << "(),\n";
	source << "\td(new " << className << "Data())\n"
		   << "{}\n\n"
		   << className << "::" << className << "(const " << className << " &other) :\n";
	if(!parent.isEmpty())
		source << "\t" << parent << "(other),\n";
	source << "\td(other.d)\n"
		   << "{}\n\n"
		   << className << "::~" << className << "() {}\n"
		   << "\n" << className << " &" << className << "::operator =(const " << className << " &other)\n"
		   << "{\n"
		   << "\td = other.d;\n"
		   << "\treturn *this;\n"
		   << "}\n";
	writeReadDefinitions(true);
	writeWriteDefinitions(true);
	if(root["$registerConverters"].toBool(true))
		writeListConverter(false);
}

void ObjectBuilder::readMembers()
{
	for(auto it = root.constBegin(); it != root.constEnd(); it++) {
		if(it.key().startsWith("$"))
			continue;
		members.insert(it.key(), it.value().toString());
	}
}

QString ObjectBuilder::setter(const QString &name)
{
	QString setterName = "set" + name;
	setterName[3] = setterName[3].toUpper();
	return setterName;
}

void ObjectBuilder::writeEnums()
{
	auto enums = root["$enums"].toObject();
	for(auto it = enums.constBegin(); it != enums.constEnd(); it++) {
		auto isFlags = false;
		QString base;
		QJsonArray values;
		if(it.value().isObject()) {
			auto obj = it->toObject();
			isFlags = obj["isFlags"].toBool(false);
			base = obj["base"].toString();
			values = obj["values"].toArray();
		} else
			values = it->toArray();

		if(!base.isEmpty())
			header << "\tenum " << it.key() << " : " << base << " {\n";
		else
			header << "\tenum " << it.key() << " {\n";
		foreach (auto value, values) {
			auto data = value.toString().split(':');
			if(data.size() == 1)
				header << "\t\t" << data[0] << ",\n";
			else if(data.size() == 2)
				header << "\t\t" << data[0] << " = " << data[1] << ",\n";
			else
				throw QStringLiteral("Enum values can have at most 2 elments!");
		}
		header << "\t};\n";

		if(isFlags) {
			header << "\tQ_DECLARE_FLAGS(" << it.key() << "s, " << it.key() << ")\n"
				   << "\tQ_FLAG(" << it.key() << "s)\n\n";
		} else
			header << "\tQ_ENUM(" << it.key() << ")\n\n";
	}
}

void ObjectBuilder::writeProperties(bool withNotify)
{
	for(auto it = members.constBegin(); it != members.constEnd(); it++) {
		header << "\tQ_PROPERTY(" << it.value() << " " << it.key()
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
		header << "\t" << it.value() << " " << it.key() << "() const;\n";
}

void ObjectBuilder::writeWriteDeclarations()
{
	for(auto it = members.constBegin(); it != members.constEnd(); it++)
		header << "\tvoid " << setter(it.key()) << "(" << it.value() << " " << it.key() << ");\n";
}

void ObjectBuilder::writeNotifyDeclarations()
{
	for(auto it = members.constBegin(); it != members.constEnd(); it++)
		header << "\tvoid " << it.key() << "Changed(" << it.value() << " " << it.key() << ");\n";
}

void ObjectBuilder::writeMemberDeclarations(QTextStream &stream)
{
	for(auto it = members.constBegin(); it != members.constEnd(); it++)
		stream << "\t" << it.value() << " _" << it.key() << ";\n";
}

void ObjectBuilder::writeSourceIncludes()
{
	source << "#include \"" << fileName << ".h\"\n\n"
		   << "#include <QtCore/qcoreapplication.h>\n"
		   << "#include <QtJsonSerializer/qjsonserializer.h>\n\n";
}

void ObjectBuilder::writeReadDefinitions(bool asGadget)
{
	auto prefix = asGadget ? QStringLiteral("d->_") : QStringLiteral("_");
	for(auto it = members.constBegin(); it != members.constEnd(); it++) {
		source << "\n" << it.value() << " " << className << "::" << it.key() << "() const\n"
			   << "{\n"
			   << "\treturn " << prefix << it.key() << ";\n"
			   << "}\n";
	}
}

void ObjectBuilder::writeWriteDefinitions(bool asGadget)
{
	auto prefix = asGadget ? QStringLiteral("d->_") : QStringLiteral("_");
	for(auto it = members.constBegin(); it != members.constEnd(); it++) {
		source << "\nvoid " << className << "::" << setter(it.key()) << "(" << it.value() << " " << it.key() << ")\n"
			   << "{\n"
			   << "\tif(" << prefix << it.key() << " == " << it.key() << ")\n"
			   << "\t\treturn;\n\n"
			   << "\t" << prefix << it.key() << " = " << it.key() <<";\n";
		if(!asGadget)
			source << "\temit " << it.key() << "Changed(" << it.key() << ");\n";
		source << "}\n";
	}
}

void ObjectBuilder::writeDataClass()
{
	auto name = className + "Data";
	source << "class " << name << " : public QSharedData\n"
		   << "{\n"
		   << "public:\n"
		   << "\t" << name << "();\n"
		   << "\t" << name << "(const " << name << " &other);\n\n";
	writeMemberDeclarations(source);
	source << "};\n\n"
		   << name << "::" << name << "() :\n"
		   << "\tQSharedData()\n";
	writeMemberDefinitions(source);
	source << "{}\n\n"
		   << name << "::" << name << "(const " << name << " &other) :\n"
		   << "\tQSharedData(other)\n";
	writeMemberCopyDefinitions(source);
	source << "{}\n\n";
}

void ObjectBuilder::writeMemberDefinitions(QTextStream &stream)
{
	for(auto it = members.constBegin(); it != members.constEnd(); it++)
		stream << "\t,_" << it.key() << "()\n";
}

void ObjectBuilder::writeMemberCopyDefinitions(QTextStream &stream)
{
	for(auto it = members.constBegin(); it != members.constEnd(); it++)
		stream << "\t,_" << it.key() << "(other._" << it.key() << ")\n";
}

void ObjectBuilder::writeListConverter(bool isObject)
{
	source << "\nvoid __" << className << "_list_conv_registrator()\n"
		   << "{\n"
		   << "\tQJsonSerializer::registerListConverters<" << className << (isObject ? "*" : "") << ">();\n"
		   << "}\n"
		   << "Q_COREAPP_STARTUP_FUNCTION(__" << className << "_list_conv_registrator)\n";
}
