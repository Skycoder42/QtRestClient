#include "objectbuilder.h"

#include <QFileInfo>
#include <QJsonArray>

#define toBool(defaultValue) .toString() == ""

ObjectBuilder::ObjectBuilder(QXmlStreamReader &inStream, QObject *parent) :
	RestBuilder(inStream, parent),
	members(),
	testEquality(true)
{}

bool ObjectBuilder::canReadType(const QString &type)
{
	return type == QStringLiteral("RestObject") ||
			type == QStringLiteral("RestGadget");
}

void ObjectBuilder::build()
{
	testEquality = readAttrib<bool>(QStringLiteral("testEquality"), true);
	if(reader.name() == QStringLiteral("RestObject"))
		generateApiObject();
	else if(reader.name() == QStringLiteral("RestGadget"))
		generateApiGadget();
	else
		Q_UNREACHABLE();
}

void ObjectBuilder::generateApiObject()
{
	auto base = readAttrib(QStringLiteral("base"), QStringLiteral("QObject"));
	auto registerConverters = readAttrib<bool>(QStringLiteral("registerConverters"), true);
	auto generateEquals = readAttrib<bool>(QStringLiteral("generateEquals"), false);

	transformIncludes({
						  QStringLiteral("QtCore/QObject"),
						  QStringLiteral("QtCore/QString")
					  });

	//write header
	header << "class " << exportedClassName << " : public " << base << "\n"
		   << "{\n"
		   << "\tQ_OBJECT\n\n";
//	writeProperties(true);
//	header << "\npublic:\n";
//	if(root.contains(QStringLiteral("$enums")))
//		writeEnums();
//	header << "\tQ_INVOKABLE " << className << "(QObject *parent = nullptr);\n\n";
//	writeReadDeclarations();
//	if(root[QStringLiteral("$generateEquals")].toBool(false))
//		writeEqualsDeclaration(false);
//	header << "\npublic Q_SLOTS:\n";
//	writeWriteDeclarations();
//	header << "\nQ_SIGNALS:\n";
//	writeNotifyDeclarations();
//	header << "\nprivate:\n";
//	writeMemberDeclarations(header);
//	header << "};\n\n";
//	writeFlagOperators();

//	//write source
//	writeSourceIncludes();
//	source << className << "::" << className << "(QObject *parent) :\n"
//		   << "\t" << parent << "(parent)\n";
//	writeMemberDefinitions(source);
//	source << "{}\n";
//	writeReadDefinitions(false);
//	writeWriteDefinitions(false);
//	if(root[QStringLiteral("$generateEquals")].toBool(false))
//		writeEqualsDefinition(false);
//	if(root[QStringLiteral("$registerConverters")].toBool(true))
//		writeListConverter(false);
}

void ObjectBuilder::generateApiGadget()
{
//	auto includes = readIncludes();
//	includes.append(QStringLiteral("QtCore/qobject.h"));
//	includes.append(QStringLiteral("QtCore/qshareddata.h"));
//	includes.append(QStringLiteral("QtCore/qstring.h"));
//	readMembers();
//	auto parent = root[QStringLiteral("$parent")].toString();

//	//write header
//	writeIncludes(header, includes);
//	header << "class " << className << "Data;\n";
//	if(parent.isEmpty())
//		header << "class " << exportedClassName << "\n";
//	else
//		header << "class " << exportedClassName << " : public " << parent << "\n";
//	header << "{\n"
//		   << "\tQ_GADGET\n\n";
//	writeProperties(false);
//	header << "\npublic:\n";
//	if(root.contains(QStringLiteral("$enums")))
//		writeEnums();
//	header << "\t" << className << "();\n"
//		   << "\t" << className << "(const " << className << " &other);\n"
//		   << "\t~" << className << "();\n\n"
//		   << "\t" << className << " &operator =(const " << className << " &other);\n\n";
//	writeReadDeclarations();
//	header << "\n";
//	writeWriteDeclarations();
//	if(root[QStringLiteral("$generateEquals")].toBool(true))
//		writeEqualsDeclaration(true);
//	header << "\nprivate:\n"
//		   << "\t QSharedDataPointer<" << className << "Data> d;\n"
//		   << "};\n\n";
//	writeFlagOperators();

//	//write source
//	writeSourceIncludes();
//	writeDataClass();
//	source << className << "::" << className << "() :\n";
//	if(!parent.isEmpty())
//		source << "\t" << parent << "(),\n";
//	source << "\td(new " << className << "Data())\n"
//		   << "{}\n\n"
//		   << className << "::" << className << "(const " << className << " &other) :\n";
//	if(!parent.isEmpty())
//		source << "\t" << parent << "(other),\n";
//	source << "\td(other.d)\n"
//		   << "{}\n\n"
//		   << className << "::~" << className << "() {}\n"
//		   << "\n" << className << " &" << className << "::operator =(const " << className << " &other)\n"
//		   << "{\n"
//		   << "\td = other.d;\n"
//		   << "\treturn *this;\n"
//		   << "}\n";
//	writeReadDefinitions(true);
//	writeWriteDefinitions(true);
//	if(root[QStringLiteral("$generateEquals")].toBool(true))
//		writeEqualsDefinition(true);
//	if(root[QStringLiteral("$registerConverters")].toBool(true))
//		writeListConverter(true);
}

void ObjectBuilder::readMembers()
{
//	for(auto it = root.constBegin(); it != root.constEnd(); it++) {
//		if(it.key().startsWith(QLatin1Char('$')))
//			continue;
//		members.insert(it.key(), it.value().toString());
//	}
}

QString ObjectBuilder::setter(const QString &name)
{
//	QString setterName = QStringLiteral("set") + name;
//	setterName[3] = setterName[3].toUpper();
//	return setterName;
}

void ObjectBuilder::writeEnums()
{
//	auto enums = root[QStringLiteral("$enums")].toObject();
//	for(auto it = enums.constBegin(); it != enums.constEnd(); it++) {
//		auto isFlags = false;
//		QString base;
//		QJsonArray values;
//		if(it.value().isObject()) {
//			auto obj = it->toObject();
//			isFlags = obj[QStringLiteral("isFlags")].toBool(false);
//			base = obj[QStringLiteral("base")].toString();
//			values = obj[QStringLiteral("values")].toArray();
//		} else
//			values = it->toArray();

//		if(!base.isEmpty())
//			header << "\tenum " << it.key() << " : " << base << " {\n";
//		else
//			header << "\tenum " << it.key() << " {\n";
//		for (auto value : values) {
//			auto data = value.toString().split(QLatin1Char(':'));
//			if(data.size() == 1)
//				header << "\t\t" << data[0] << ",\n";
//			else if(data.size() == 2)
//				header << "\t\t" << data[0] << " = " << data[1] << ",\n";
//			else
//				throw tr("Enum values can have at most 2 elments!");
//		}
//		header << "\t};\n";

//		if(isFlags) {
//			header << "\tQ_DECLARE_FLAGS(" << it.key() << "s, " << it.key() << ")\n"
//				   << "\tQ_FLAG(" << it.key() << "s)\n\n";
//		} else
//			header << "\tQ_ENUM(" << it.key() << ")\n\n";
//	}
}

void ObjectBuilder::writeFlagOperators()
{
//	auto hasFlags = false;
//	auto enums = root[QStringLiteral("$enums")].toObject();
//	for(auto it = enums.constBegin(); it != enums.constEnd(); it++) {
//		auto isFlags = false;
//		QString base;
//		QJsonArray values;
//		if(it.value().isObject()) {
//			auto obj = it->toObject();
//			isFlags = obj[QStringLiteral("isFlags")].toBool(false);
//			base = obj[QStringLiteral("base")].toString();
//			values = obj[QStringLiteral("values")].toArray();
//		} else
//			values = it->toArray();

//		if(isFlags){
//			hasFlags = true;
//			header << "Q_DECLARE_OPERATORS_FOR_FLAGS(" << className << "::" << it.key() << "s)\n";
//		}
//	}

//	if(hasFlags)
//		header << "\n";
}

void ObjectBuilder::writeProperties(bool withNotify)
{
//	for(auto it = members.constBegin(); it != members.constEnd(); it++) {
//		header << "\tQ_PROPERTY(" << it.value() << " " << it.key()
//			   << " READ " << it.key()
//			   << " WRITE " << setter(it.key());
//		if(withNotify)
//			header << " NOTIFY " << it.key() << "Changed";
//		header << ")\n";
//	}
}

void ObjectBuilder::writeReadDeclarations()
{
//	for(auto it = members.constBegin(); it != members.constEnd(); it++)
//		header << "\t" << it.value() << " " << it.key() << "() const;\n";
}

void ObjectBuilder::writeWriteDeclarations()
{
//	for(auto it = members.constBegin(); it != members.constEnd(); it++)
//		header << "\tvoid " << setter(it.key()) << "(" << it.value() << " " << it.key() << ");\n";
}

void ObjectBuilder::writeEqualsDeclaration(bool asGadget)
{
//	header << "\n\tbool operator ==(const " << className << " " << (asGadget ? "&" : "*") << "other) const;\n";
//	header << "\tbool operator !=(const " << className << " " << (asGadget ? "&" : "*") << "other) const;\n";
}

void ObjectBuilder::writeNotifyDeclarations()
{
//	for(auto it = members.constBegin(); it != members.constEnd(); it++)
//		header << "\tvoid " << it.key() << "Changed(" << it.value() << " " << it.key() << ");\n";
}

void ObjectBuilder::writeMemberDeclarations(QTextStream &stream)
{
//	for(auto it = members.constBegin(); it != members.constEnd(); it++)
//		stream << "\t" << it.value() << " _" << it.key() << ";\n";
}

void ObjectBuilder::writeSourceIncludes()
{
//	source << "#include \"" << fileName << ".h\"\n\n"
//		   << "#include <QtCore/qcoreapplication.h>\n"
//		   << "#include <QtJsonSerializer/qjsonserializer.h>\n\n";
}

void ObjectBuilder::writeReadDefinitions(bool asGadget)
{
//	auto prefix = asGadget ? QStringLiteral("d->_") : QStringLiteral("_");
//	for(auto it = members.constBegin(); it != members.constEnd(); it++) {
//		source << "\n" << it.value() << " " << className << "::" << it.key() << "() const\n"
//			   << "{\n"
//			   << "\treturn " << prefix << it.key() << ";\n"
//			   << "}\n";
//	}
}

void ObjectBuilder::writeWriteDefinitions(bool asGadget)
{
//	auto prefix = asGadget ? QStringLiteral("d->_") : QStringLiteral("_");
//	for(auto it = members.constBegin(); it != members.constEnd(); it++) {
//		source << "\nvoid " << className << "::" << setter(it.key()) << "(" << it.value() << " " << it.key() << ")\n"
//			   << "{\n";
//		if(testEquality) {
//			source << "\tif(" << prefix << it.key() << " == " << it.key() << ")\n"
//				   << "\t\treturn;\n\n";
//		}
//		source << "\t" << prefix << it.key() << " = " << it.key() <<";\n";
//		if(!asGadget)
//			source << "\temit " << it.key() << "Changed(" << it.key() << ");\n";
//		source << "}\n";
//	}
}

void ObjectBuilder::writeEqualsDefinition(bool asGadget)
{
//	QString prefix = asGadget ? QStringLiteral("d->_") : QStringLiteral("_");
//	QString otherPrefix = (asGadget ? QStringLiteral(".") : QStringLiteral("->")) + prefix;
//	//equals
//	source << "\nbool " << className << "::" << "operator ==(const " << className << " " << (asGadget ? "&" : "*") << "other) const\n"
//		   << "{\n"
//		   << "\treturn true";
//	for(auto it = members.constBegin(); it != members.constEnd(); it++)
//		source << "\n\t\t&& " << prefix << it.key() << " == other" << otherPrefix << it.key();
//	source << ";\n"
//		   << "}\n";

//	//unequals
//	source << "\nbool " << className << "::" << "operator !=(const " << className << " " << (asGadget ? "&" : "*") << "other) const\n"
//		   << "{\n"
//		   << "\treturn false";
//	for(auto it = members.constBegin(); it != members.constEnd(); it++)
//		source << "\n\t\t|| " << prefix << it.key() << " != other" << otherPrefix << it.key();
//	source << ";\n"
//		   << "}\n";
}

void ObjectBuilder::writeDataClass()
{
//	QString name = className + QStringLiteral("Data");
//	source << "class " << name << " : public QSharedData\n"
//		   << "{\n"
//		   << "public:\n"
//		   << "\t" << name << "();\n"
//		   << "\t" << name << "(const " << name << " &other);\n\n";
//	writeMemberDeclarations(source);
//	source << "};\n\n"
//		   << name << "::" << name << "() :\n"
//		   << "\tQSharedData()\n";
//	writeMemberDefinitions(source);
//	source << "{}\n\n"
//		   << name << "::" << name << "(const " << name << " &other) :\n"
//		   << "\tQSharedData(other)\n";
//	writeMemberCopyDefinitions(source);
//	source << "{}\n\n";
}

void ObjectBuilder::writeMemberDefinitions(QTextStream &stream)
{
//	for(auto it = members.constBegin(); it != members.constEnd(); it++)
//		stream << "\t,_" << it.key() << "()\n";
}

void ObjectBuilder::writeMemberCopyDefinitions(QTextStream &stream)
{
//	for(auto it = members.constBegin(); it != members.constEnd(); it++)
//		stream << "\t,_" << it.key() << "(other._" << it.key() << ")\n";
}

void ObjectBuilder::writeListConverter(bool asGadget)
{
//	source << "\nvoid __" << className << "_list_conv_registrator()\n"
//		   << "{\n"
//		   << "\tQJsonSerializer::registerListConverters<" << className << (asGadget ? "" : "*") << ">();\n"
//		   << "}\n"
//		   << "Q_COREAPP_STARTUP_FUNCTION(__" << className << "_list_conv_registrator)\n";
}
