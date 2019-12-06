#include "objectbuilder.h"

#include <QFileInfo>
#include <QJsonArray>
#include <QVersionNumber>

ObjectBuilder::ObjectBuilder(RestBuilderXmlReader::RestObject restObject) :
	isObject{true},
	data{std::move(restObject)}
{}

ObjectBuilder::ObjectBuilder(RestBuilderXmlReader::RestGadget restGadget) :
	isObject{false},
	data{std::move(restGadget)}
{}

void ObjectBuilder::build()
{
	// data  preprocessing
	data.includes = QList<RestBuilderXmlReader::Include>{
		{false, QStringLiteral("QtCore/QObject")},
		{false, QStringLiteral("QtCore/QString")},
		{false, isObject ? QStringLiteral("QtCore/QScopedPointer") : QStringLiteral("QtCore/QSharedData")}
	} + data.includes;
	if(isObject && !data.base)
		data.base = QStringLiteral("QObject");

	// cpp code generation
	if(isObject)
		generateApiObject();
	else
		generateApiGadget();
}

void ObjectBuilder::generateApiObject()
{
	//write header
	writeIncludes(data.includes);
	if(data.nspace)
		header << "namespace " << data.nspace.value() << " {\n\n";
	header << "class " << data.name << "Private;\n"
		   << "class " << exportedName(data.name, data.exportKey) << " : public " << data.base.value() << "\n"
		   << "{\n"
		   << "\tQ_OBJECT\n\n";
	writeProperties();
	header << "\npublic:\n";
	if(!data.enums.isEmpty())
		writeEnums();
	header << "\tQ_INVOKABLE explicit " << data.name << "(QObject *parent = nullptr);\n";
	writeAggregateConstructorDeclaration();
	header << "\t~" << data.name << "() override;\n\n";
	writeReadDeclarations();
	if(data.generateEquals.value_or(false))
		writeEqualsDeclaration();
	header << "\npublic Q_SLOTS:\n";
	writeWriteDeclarations();
	writeResetDeclarations();

	header << "\nQ_SIGNALS:\n";
	for(const auto &propVar : qAsConst(data.properties)) {
		if (nonstd::holds_alternative<RestBuilderXmlReader::Property>(propVar)) {
			const auto &prop = nonstd::get<RestBuilderXmlReader::Property>(propVar);
			header << "\tvoid " << prop.key << "Changed(const " << prop.metaType.value_or(prop.type) << " &" << prop.key << ");\n";
		} else {
			const auto &prop = nonstd::get<RestBuilderXmlReader::UserProperty>(propVar);
			if (prop.notify && prop.notify->declare)
				header << "\tvoid " << prop.notify->name << "(const " << prop.metaType.value_or(prop.type) << " &" << prop.key << ");\n";
		}
	}
	header << "\nprivate:\n"
		   << "\tQScopedPointer<" << data.name << "Private> d;\n"
		   << "};\n\n";
	if(data.nspace)
		header << "}\n\n";
	writeFlagOperators();

	//write source
	writeSourceIncludes();
	writePrivateClass();
	source << data.name << "::" << data.name << "(QObject *parent) :\n"
		   << "\t" << data.base.value() << "{parent},\n"
		   << "\td{new " << data.name << "Private{}}\n"
		   << "{}\n\n";
	writeAggregateConstructorDefinition();
	source << data.name << "::~" << data.name << "() = default;\n\n";
	writeReadDefinitions();
	if(data.generateEquals.value_or(false))
		writeEqualsDefinition();
	writeWriteDefinitions();
	writeResetDefinitions();
	writeSetupHooks();
}

void ObjectBuilder::generateApiGadget()
{
	//write header
	writeIncludes(data.includes);
	if(data.nspace)
		header << "namespace " << data.nspace.value() << " {\n\n";
	header << "class " << data.name << "Data;\n";
	if(data.base)
		header << "class " << exportedName(data.name, data.exportKey) << " : public " << data.base.value() << "\n";
	else
		header << "class " << exportedName(data.name, data.exportKey) << "\n";
	header << "{\n"
		   << "\tQ_GADGET\n\n";
	writeProperties();
	header << "\npublic:\n";
	if(!data.enums.isEmpty())
		writeEnums();
	header << "\t" << data.name << "();\n";
	writeAggregateConstructorDeclaration();
	header << "\t" << data.name << "(const " << data.name << " &other);\n"
		   << "\t" << data.name << "(" << data.name << " &&other) noexcept;\n"
		   << "\t~" << data.name << "();\n\n"
		   << "\t" << data.name << " &operator=(const " << data.name << " &other);\n"
		   << "\t" << data.name << " &operator=(" << data.name << " &&other) noexcept;\n\n";
	writeReadDeclarations();
	header << '\n';
	writeWriteDeclarations();
	writeResetDeclarations();
	if(data.generateEquals.value_or(true))
		writeEqualsDeclaration();
	header << "\nprivate:\n";
	if (data.generateEquals.value_or(true))
		writeQHashDeclaration(true);
	header << "\tQSharedDataPointer<" << data.name << "Data> d;\n"
		   << "};\n\n";
	if (data.generateEquals.value_or(true))
		writeQHashDeclaration(false);
	if(data.nspace)
		header << "}\n\n";
	header << "Q_DECLARE_METATYPE(" << nsName(data.name, data.nspace) << ")\n\n";
	writeFlagOperators();

	//write source
	writeSourceIncludes();
	writeDataClass();
	source << data.name << "::" << data.name << "() :\n";
	if(data.base)
		source << "\t" << data.base.value() << "{},\n";
	source << "\td{new " << data.name << "Data{}}\n"
		   << "{}\n\n";
	writeAggregateConstructorDefinition();
	source << data.name << "::" << data.name << "(const " << data.name << " &other) = default;\n\n"
		   << data.name << "::" << data.name << "(" << data.name << " &&other) noexcept = default;\n\n"
		   << data.name << "::~" << data.name << "() = default;\n\n"
		   << data.name << " &" << data.name << "::operator=(const " << data.name << " &other) = default;\n\n"
		   << data.name << " &" << data.name << "::operator=(" << data.name << " &&other) noexcept = default;\n";
	writeReadDefinitions();
	writeWriteDefinitions();
	writeResetDefinitions();
	if(data.generateEquals.value_or(true)) {
		writeEqualsDefinition();
		writeQHashDefinition();
	}
	writeSetupHooks();
}

QString ObjectBuilder::setter(const QString &name)
{
	return QStringLiteral("set") + name.mid(0, 1).toUpper() + name.mid(1);
}

const RestBuilderXmlReader::TypedVariableAttribs &ObjectBuilder::propertyBasics(const nonstd::variant<RestBuilderXmlReader::Property, RestBuilderXmlReader::UserProperty> &prop) const
{
	if (nonstd::holds_alternative<RestBuilderXmlReader::Property>(prop))
		return nonstd::get<RestBuilderXmlReader::Property>(prop);
	else
		return nonstd::get<RestBuilderXmlReader::UserProperty>(prop);
}

const RestBuilderXmlReader::PropertyAttribs &ObjectBuilder::propertyAttribs(const nonstd::variant<RestBuilderXmlReader::Property, RestBuilderXmlReader::UserProperty> &prop) const
{
	if (nonstd::holds_alternative<RestBuilderXmlReader::Property>(prop))
		return nonstd::get<RestBuilderXmlReader::Property>(prop);
	else
		return nonstd::get<RestBuilderXmlReader::UserProperty>(prop);
}

ObjectBuilder::SudoProperty ObjectBuilder::sudoProperty(const nonstd::variant<RestBuilderXmlReader::Property, RestBuilderXmlReader::UserProperty> &prop) const
{
	return {
		propertyBasics(prop),
		propertyAttribs(prop)
	};
}

void ObjectBuilder::writeEnums()
{
	for(const auto &eElem : qAsConst(data.enums)) {
		if(!eElem.base.isEmpty())
			header << "\tenum " << (eElem.isClass ? "class " : "") << eElem.name << " : " << eElem.base << " {\n";
		else
			header << "\tenum " << (eElem.isClass ? "class " : "") << eElem.name << " {\n";
		for(const auto &value : eElem.keys) {
			if(value.value.isEmpty())
				header << "\t\t" << value.name << ",\n";
			else
				header << "\t\t" << value.name << " = " << value.value << ",\n";
		}
		header << "\t};\n";

		if(eElem.isFlags) {
			header << "\tQ_DECLARE_FLAGS(" << eElem.name << "s, " << eElem.name << ")\n"
				   << "\tQ_FLAG(" << eElem.name << "s)\n\n";
		} else
			header << "\tQ_ENUM(" << eElem.name << ")\n\n";
	}
}

void ObjectBuilder::writeFlagOperators()
{
	auto hasFlags = false;
	for(const auto &eElem : qAsConst(data.enums)) {
		if(eElem.isFlags){
			hasFlags = true;
			header << "Q_DECLARE_OPERATORS_FOR_FLAGS(" << nsName(data.name, data.nspace) << "::" << eElem.name << "s)\n";
		}
	}

	if(hasFlags)
		header << "\n";
}

void ObjectBuilder::writeProperties()
{
	for(const auto &propVar : qAsConst(data.properties)) {
		const RestBuilderXmlReader::PropertyAttribs *attribs;
		if (nonstd::holds_alternative<RestBuilderXmlReader::Property>(propVar)) {
			const auto &prop = nonstd::get<RestBuilderXmlReader::Property>(propVar);
			header << "\tQ_PROPERTY(" << prop.metaType.value_or(prop.type) << " " << prop.key
				   << " READ " << prop.key
				   << " WRITE " << setter(prop.key);
			if(prop.generateReset)
				header << " RESET re" << setter(prop.key);
			if(isObject)
				header << " NOTIFY " << prop.key << "Changed";
			attribs = &prop;
		} else {
			const auto &prop = nonstd::get<RestBuilderXmlReader::UserProperty>(propVar);
			header << "\tQ_PROPERTY(" << prop.metaType.value_or(prop.type) << " " << prop.key
				   << " READ " << prop.read.name;
			if (prop.write)
				header << " WRITE " << prop.write->name;
			if (prop.reset)
				header << " RESET " << prop.reset->name;
			if (prop.notify)
				header << " NOTIFY " << prop.notify->name;
			if (!prop.write && !prop.reset && !prop.notify)
				header << " CONSTANT";
			attribs = &prop;
		}

		if (attribs->revision)
			header << " REVISION " << *attribs->revision;
		if (attribs->designable)
			header << " DESIGNABLE " << boolValue(*attribs->designable);
		if (attribs->scriptable)
			header << " SCRIPTABLE " << boolValue(*attribs->scriptable);
		if (attribs->stored)
			header << " STORED " << boolValue(*attribs->stored);
		if (attribs->user)
			header << " USER " << boolValue(*attribs->user);
		if (attribs->final)
			header << " FINAL";
		header << ")\n";
	}
}

void ObjectBuilder::writeAggregateConstructorDeclaration()
{
	auto offsetSpaces = data.name.size() + 1;
	auto offsetTabs = 1 + offsetSpaces / 4;
	offsetSpaces %= 4;
	header << "\t" << data.name << "(";
	auto isFirst = true;
	for (const auto &propVar : qAsConst(data.properties)) {
		const auto &propBase = propertyBasics(propVar);
		if (isFirst) {
			isFirst = false;
			header << propBase.type << " " << propBase.key;
		} else {
			header << ",\n"
				   << QByteArray(offsetTabs, '\t')
				   << QByteArray(offsetSpaces, ' ')
				   << propBase.type << " " << propBase.key;
			if (data.aggregateDefaults) {
				if (nonstd::holds_alternative<RestBuilderXmlReader::Property>(propVar)) {
					const auto &prop = nonstd::get<RestBuilderXmlReader::Property>(propVar);
					if (prop.defaultValue.isEmpty())
						header << " = {}";
					else
						header << " = " << writeParamDefault(prop);
				} else {
					const auto &prop = nonstd::get<RestBuilderXmlReader::UserProperty>(propVar);
					if (prop.defaultValue)
						header << " = " << writeExpression(*prop.defaultValue, true);
					else
						header << " = {}";
				}

			}
		}
	}

	if (isObject) {
		header << ",\n"
			   << QByteArray(offsetTabs, '\t')
			   << QByteArray(offsetSpaces, ' ')
			   << "QObject *parent = nullptr";
	}

	header << ");\n";
}

void ObjectBuilder::writeReadDeclarations()
{
	for(const auto &propVar : qAsConst(data.properties)) {
		if (nonstd::holds_alternative<RestBuilderXmlReader::Property>(propVar)) {
			const auto &prop = nonstd::get<RestBuilderXmlReader::Property>(propVar);
			header << "\t" << prop.metaType.value_or(prop.type) << " " << prop.key << "() const;\n";
		} else {
			const auto &prop = nonstd::get<RestBuilderXmlReader::UserProperty>(propVar);
			if (!prop.read.definition.isEmpty()) {
				header << "\t";
				if (prop.read.invokable)
					header << "Q_INVOKABLE ";
				if (prop.read.isVirtual)
					header << "virtual ";
				header << prop.metaType.value_or(prop.type) << " " << prop.read.name << "() const;\n";
			}
		}
	}
	if(data.simpleHref)
		header << "\n\tQUrl extensionHref() const override;\n";
}

void ObjectBuilder::writeWriteDeclarations()
{
	for(const auto &propVar : qAsConst(data.properties)) {
		if (nonstd::holds_alternative<RestBuilderXmlReader::Property>(propVar)) {
			const auto &prop = nonstd::get<RestBuilderXmlReader::Property>(propVar);
			header << "\tvoid " << setter(prop.key) << "(" << prop.metaType.value_or(prop.type) << " " << prop.key << ");\n";
		} else {
			const auto &prop = nonstd::get<RestBuilderXmlReader::UserProperty>(propVar);
			if (prop.write && !prop.write->definition.isEmpty()) {
				header << "\t";
				if (prop.write->isVirtual)
					header << "virtual ";
				header << "void " << prop.write->name << "(" << prop.metaType.value_or(prop.type) << " " << prop.write->parameter << ");\n";
			}
		}
	}
}

void ObjectBuilder::writeResetDeclarations()
{
	auto once = true;
	for(const auto &propVar : qAsConst(data.properties)) {
		if (nonstd::holds_alternative<RestBuilderXmlReader::Property>(propVar)) {
			const auto &prop = nonstd::get<RestBuilderXmlReader::Property>(propVar);
			if(!prop.generateReset)
				continue;
			if(once) {
				once = false;
				header << '\n';
			}
			header << "\tvoid re" << setter(prop.key) << "();\n";
		} else {
			const auto &prop = nonstd::get<RestBuilderXmlReader::UserProperty>(propVar);
			if (prop.reset && !prop.reset->definition.isEmpty()) {
				if(once) {
					once = false;
					header << '\n';
				}

				header << "\t";
				if (prop.reset->isVirtual)
					header << "virtual ";
				header << "void " << prop.reset->name << "();\n";
			}
		}
	}
}

void ObjectBuilder::writeEqualsDeclaration()
{
	if(isObject)
		header << "\n\tQ_INVOKABLE bool equals(const " << data.name << " *other) const;\n";
	else {
		header << "\n\tbool operator==(const " << data.name << " &other) const;\n";
		header << "\tbool operator!=(const " << data.name << " &other) const;\n";
	}
}

void ObjectBuilder::writeQHashDeclaration(bool asFriend)
{
	if (asFriend)
		header << "\tfriend ";
	header << exportedName(QStringLiteral("uint qHash"), data.exportKey)
		   << "(const " << data.name << " &data, uint seed" << (asFriend ? "" : " = 0") << ");\n\n";
}

void ObjectBuilder::writeSourceIncludes()
{
	source << "#include \"" << fileName << ".h\"\n\n"
		   << "#include <limits>\n"
		   << "#include <QtCore/QVariant>\n";
	if(data.registerConverters) {
		source << "#include <QtCore/QCoreApplication>\n"
			   << "#include <QtCore/QHash>\n"
			   << "#include <QtJsonSerializer/SerializerBase>\n";
	}
	if(data.qmlUri)
		source << "#include <QtQml/qqml.h>\n";
	if(data.nspace)
		source << "using namespace " << data.nspace.value() << ";\n";
					source << '\n';
}

void ObjectBuilder::writeAggregateConstructorDefinition()
{
	auto isFirst = true;
	source << data.name << "::" << data.name << "(";
	for (const auto &propVar : qAsConst(data.properties)) {
		const auto &prop = propertyBasics(propVar);
		if (isFirst)
			isFirst = false;
		else
			source << ", ";
		source << prop.type << " " << prop.key;
	}
	if (isObject)
		source << ", QObject *parent";
	source << ") :\n";

	if (isObject) {
		source << "\t" << data.base.value() << "{parent},\n"
			   << "\td{new " << data.name << "Private{";
		isFirst = true;
	} else {
		if(data.base)
			source << "\t" << data.base.value() << "{},\n";
		source << "\td{new " << data.name << "Data{QSharedData{}";
	}

	for (const auto &propVar : qAsConst(data.properties)) {
		const auto &prop = propertyBasics(propVar);
		if (isFirst)
			isFirst = false;
		else
			source << ", ";
		source << "std::move(" << prop.key << ")";
	}

	source << "}}\n"
		   << "{}\n\n";
}

void ObjectBuilder::writeReadDefinitions()
{
	for(const auto &propVar : qAsConst(data.properties)) {
		if (nonstd::holds_alternative<RestBuilderXmlReader::Property>(propVar)) {
			const auto &prop = nonstd::get<RestBuilderXmlReader::Property>(propVar);
			source << "\n" << prop.type << " " << data.name << "::" << prop.key << "() const\n"
				   << "{\n"
				   << "\treturn d->" << prop.key << ";\n"
				   << "}\n";
		} else {
			const auto &prop = nonstd::get<RestBuilderXmlReader::UserProperty>(propVar);
			if (!prop.read.definition.isEmpty()) {
				source << "\n" << prop.type << " " << data.name << "::" << prop.read.name << "() const\n"
					   << "{\n\t"
					   << prop.read.definition.trimmed()
					   << "\n}\n";
			}
		}
	}

	if(data.simpleHref) {
		source << "\nQUrl " << data.name << "::extensionHref() const\n"
			   << "{\n"
			   << "\treturn d->" << data.simpleHref.value() << ";\n"
			   << "}\n";
	}
}

void ObjectBuilder::writeWriteDefinitions()
{
	for(const auto &propVar : qAsConst(data.properties)) {
		if (nonstd::holds_alternative<RestBuilderXmlReader::Property>(propVar)) {
			const auto &prop = nonstd::get<RestBuilderXmlReader::Property>(propVar);
			source << "\nvoid " << data.name << "::" << setter(prop.key) << "(" << prop.type << " " << prop.key << ")\n"
				   << "{\n";
			if(data.testEquality) {
				source << "\tif(d->" << prop.key << " == " << prop.key << ")\n"
					   << "\t\treturn;\n\n";
			}
			source << "\td->" << prop.key << " = std::move(" << prop.key <<");\n";
			if(isObject)
				source << "\temit " << prop.key << "Changed(d->" << prop.key << ");\n";
			source << "}\n";
		} else {
			const auto &prop = nonstd::get<RestBuilderXmlReader::UserProperty>(propVar);
			if (prop.write && !prop.write->definition.isEmpty()) {
				source << "\nvoid " << data.name << "::" << prop.write->name << "(" << prop.type << " " << prop.write->parameter << ")\n"
					   << "{\n\t"
					   << prop.write->definition.trimmed()
					   << "\n}\n";
			}
		}
	}
}

void ObjectBuilder::writeResetDefinitions()
{
	for(const auto &propVar : qAsConst(data.properties)) {
		if (nonstd::holds_alternative<RestBuilderXmlReader::Property>(propVar)) {
			const auto &prop = nonstd::get<RestBuilderXmlReader::Property>(propVar);
			if(!prop.generateReset)
				continue;

			source << "\nvoid " << data.name << "::re" << setter(prop.key) << "()\n"
				   << "{\n"
				   << "\t" << setter(prop.key) << "(";
			if(prop.defaultValue.isEmpty())
				source << prop.type << "{}";
			else
				source << writeParamDefault(prop);
			source << ");\n"
				   << "}\n";
		} else {
			const auto &prop = nonstd::get<RestBuilderXmlReader::UserProperty>(propVar);
			if (prop.reset && !prop.reset->definition.isEmpty()) {
				source << "\nvoid " << data.name << "::" << prop.reset->name << "()\n"
					   << "{\n\t"
					   << prop.reset->definition.trimmed()
					   << "\n}\n";
			}
		}
	}
}

void ObjectBuilder::writeEqualsDefinition()
{
	QString otherPrefix = (isObject ? QStringLiteral("->") : QStringLiteral(".")) + QStringLiteral("d->");
	//equals
	if(isObject)
		source << "\nbool " << data.name << "::" << "equals(const " << data.name << " *other) const\n";
	else
		source << "\nbool " << data.name << "::" << "operator==(const " << data.name << " &other) const\n";
	source << "{\n"
		   << "\treturn true";
	for(const auto &propVar : qAsConst(data.properties)) {
		const auto &prop = propertyBasics(propVar);
		source << "\n\t\t&& d->" << prop.key << " == other" << otherPrefix << prop.key;
	}
	source << ";\n"
		   << "}\n";

	//unequals
	if(!isObject) {
		source << "\nbool " << data.name << "::" << "operator!=(const " << data.name << " &other) const\n"
			   << "{\n"
			   << "\treturn false";
		for(const auto &propVar : qAsConst(data.properties)) {
			const auto &prop = propertyBasics(propVar);
			source << "\n\t\t|| d->" << prop.key << " != other" << otherPrefix << prop.key;
		}
		source << ";\n"
			   << "}\n";
	}
}

void ObjectBuilder::writeQHashDefinition()
{
	source << "\nnamespace {\n\n"
		   << "inline uint seed_rot(uint x, uint &n) {\n"
		   << "\tn %= std::numeric_limits<uint>::digits;\n"
		   << "\tx = (x << n) | (x >> ((-n) & (std::numeric_limits<uint>::digits - 1)));\n"
		   << "\t++n;\n"
		   << "\treturn x;\n"
		   << "}\n\n"
		   << "template <typename T>\n"
		   << "inline uint dynHash(const T &data, uint seed) {\n"
		   << "\treturn qHash(data, seed);\n"
		   << "}\n\n"
		   << "}\n\n"

		   << "uint " << nsName(QStringLiteral("qHash"), data.nspace)
		   << "(const " << data.name << "& data, uint seed)\n"
		   << "{\n"
		   << "\tauto n = 0u;\n"
		   << "\treturn seed_rot(seed, n)";
	for(const auto &propVar : qAsConst(data.properties)) {
		const auto &prop = propertyBasics(propVar);
		source << "\n\t\t^ dynHash(data.d->" << prop.key << ", seed_rot(seed, n))";
	}
	source << ";\n}\n";
}

void ObjectBuilder::writePrivateClass()
{
	QString name = data.name + QStringLiteral("Private");
	if(data.nspace)
		source << "namespace " << data.nspace.value() << " {\n\n";
	source << "class " << name << "\n"
		   << "{\n"
		   << "public:\n";
	writeMemberDefinitions();
	source << "};\n\n";
	if(data.nspace)
		source << "}\n\n";
}

void ObjectBuilder::writeDataClass()
{
	QString name = data.name + QStringLiteral("Data");
	if(data.nspace)
		source << "namespace " << data.nspace.value() << " {\n\n";
	source << "class " << name << " : public QSharedData\n"
		   << "{\n"
		   << "public:\n";
	writeMemberDefinitions();
	source << "};\n\n";
	if(data.nspace)
		source << "}\n\n";
}

void ObjectBuilder::writeMemberDefinitions()
{
	for(const auto &propVar : qAsConst(data.properties)) {
		if (nonstd::holds_alternative<RestBuilderXmlReader::Property>(propVar)) {
			const auto &prop = nonstd::get<RestBuilderXmlReader::Property>(propVar);
			source << "\t" << prop.type << " " << prop.key << " {";
			if(!prop.defaultValue.isEmpty())
				source << writeParamDefault(prop);
			source << "};\n";
		} else {
			const auto &prop = nonstd::get<RestBuilderXmlReader::UserProperty>(propVar);
			source << "\t" << prop.type << " " << prop.member.value_or(prop.key) << " {";
			if(prop.defaultValue)
				source << writeExpression(*prop.defaultValue, true);
			source << "};\n";
		}
	}
}

void ObjectBuilder::writeSetupHooks()
{
	if(!data.registerConverters && !data.qmlUri)
		return;

	source << "\nnamespace {\n\n"
		   << "void __" << data.name << "_setup_hook()\n"
		   << "{\n";

	if(data.registerConverters)
		source << "\tQtJsonSerializer::SerializerBase::registerListConverters<" << data.name << (isObject ? "*" : "") << ">();\n";
	if(data.qmlUri) {
		auto uriParts = data.qmlUri.value().split(QLatin1Char(' '), QString::SkipEmptyParts);
		auto uriPath = uriParts.takeFirst();
		auto uriVersion = QVersionNumber::fromString(uriParts.join(QLatin1Char(' ')));
		if(uriVersion.isNull())
			uriVersion = {1,0};
		if(isObject) {
			source << "\tqmlRegisterType<" << data.name << ">(\""
				   << uriPath << "\", " << uriVersion.majorVersion() << ", " << uriVersion.minorVersion()
				   << ", \"" << data.name << "\");\n";
		} else {
			source << "\tqmlRegisterUncreatableType<" << data.name << ">(\""
				   << uriPath << "\", " << uriVersion.majorVersion() << ", " << uriVersion.minorVersion()
				   << ", \"" << data.name << "\", QStringLiteral(\"Q_GADGETs cannot be created from QML\"));\n";
		}
	}

	source << "}\n\n"
		   << "}\n"
		   << "Q_COREAPP_STARTUP_FUNCTION(__" << data.name << "_setup_hook)\n";
}
