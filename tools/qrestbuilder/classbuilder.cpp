#include "classbuilder.h"
#include <QJsonArray>
#include <QDebug>

ClassBuilder::ClassBuilder() {}

void ClassBuilder::build()
{
	if(root["type"].toString() == "api")
		generateApi();
	else if(root["type"].toString() == "class")
		generateClass();
	else
		throw QStringLiteral("REST_API_CLASSES must be either of type api or class");
}

QString ClassBuilder::specialPrefix()
{
	return QString();
}

void ClassBuilder::generateClass()
{
	qInfo() << "generating class:" << className;

	auto includes = readIncludes();
	includes.append("QtRestClient");
	includes.append("QString");
	includes.append("QStringList");
	auto parent = root["parent"].toString("QObject");

	readClasses();
	readMethods();

	//write header
	writeIncludes(header, includes);
	header << "class " << className << " : public " << parent << "\n"
		   << "{\n"
		   << "public:\n";
	generateFactoryDeclaration();
	header << "\t" << className << "(QtRestClient::RestClass *restClass, QObject *parent);\n\n";
	writeClassDeclarations();
	writeMethodDeclarations();
	header << "private:\n"
		   << "\tQtRestClient::RestClass *restClass;\n";
	writeMemberDeclarations();
	header << "};\n\n";

	//write source
	source << "#include \"" << fileName << ".h\"\n"
		   << "using namespace QtRestClient;\n\n"
		   << "const QString " << className << "::Path(\"" << root["path"].toString() << "\");\n";
	generateFactoryDefinition();
	source << "\n" << className << "::" << className << "(RestClass *restClass, QObject *parent) :\n"
		   << "\t" << parent << "(parent),\n"
		   << "\trestClass(restClass)\n";
	writeMemberDefinitions();
	source << "{\n"
		   << "\trestClass->setParent(this);\n"
		   << "}\n";
	writeClassDefinitions();
	writeMethodDefinitions();
}

void ClassBuilder::generateApi()
{
	qInfo() << "generating api:" << className;

	generateClass();
}

void ClassBuilder::readClasses()
{
	auto cls = root["classes"].toObject();
	for(auto it = cls.constBegin(); it != cls.constEnd(); it++)
		classes.insert(it.key(), it.value().toString());
}

void ClassBuilder::readMethods()
{
	auto member = root["methods"].toObject();
	for(auto it = member.constBegin(); it != member.constEnd(); it++) {
		auto obj = it.value().toObject();
		MethodInfo info;
		info.path = obj["path"].toString(info.path);
		info.url = obj["url"].toString(info.url);
		if(!info.path.isEmpty() && !info.url.isEmpty())
			throw QStringLiteral("You can only use either path or url, not both!");
		info.verb = obj["verb"].toString(info.verb);
		foreach(auto value, obj["pathParams"].toArray()) {
			auto param = value.toString().split(",");
			if(param.size() != 2)
				throw QStringLiteral("Element in pathParams must be ,-seperated!");
			info.pathParams.append({param[0], param[1]});
		}
		foreach(auto value, obj["parameters"].toArray()) {
			auto param = value.toString().split(",");
			if(param.size() != 2)
				throw QStringLiteral("Element in parameters must be ,-seperated!");
			info.parameters.append({param[0], param[1]});
		}
		auto headers = obj["headers"].toObject();
		for(auto jt = headers.constBegin(); jt != headers.constEnd(); jt++)
			info.headers.insert(jt.key(), jt.value().toString());
		info.body = obj["body"].toString(info.body);
		info.returns = obj["returns"].toString(info.returns);
		info.except = obj["except"].toString(info.except);

		methods.insert(it.key(), info);
	}
}

void ClassBuilder::generateFactoryDeclaration()
{
	header << "\tstatic const QString Path;\n\n"
		   << "\tclass Factory\n"
		   << "\t{\n"
		   << "\tpublic:\n"
		   << "\t\tFactory(QtRestClient::RestClient *client, const QStringList &parentPath);\n\n";
	writeFactoryDeclarations();
	header << "\t\t" << className << " *instance(QObject *parent = nullptr) const;\n\n"
		   << "\tprivate:\n"
		   << "\t\tQtRestClient::RestClient *client;\n"
		   << "\t\tQStringList subPath;\n"
		   << "\t};\n\n";
}

void ClassBuilder::writeFactoryDeclarations()
{
	for(auto it = classes.constBegin(); it != classes.constEnd(); it++)
		header << "\t\t" << it.value() << "::Factory " << it.key() << "() const;\n";
	if(!classes.isEmpty())
		header << '\n';
}

void ClassBuilder::writeClassDeclarations()
{
	for(auto it = classes.constBegin(); it != classes.constEnd(); it++)
		header << "\t" << it.value() << " *" << it.key() << "() const;\n";
	if(!classes.isEmpty())
		header << '\n';
}

void ClassBuilder::writeMethodDeclarations()
{
	for(auto it = methods.constBegin(); it != methods.constEnd(); it++) {
		header << "\tQtRestClient::GenericRestReply<" << it->returns << "," << it->except << "> *" << it.key() << "(";
		QStringList parameters;
		if(!it->body.isEmpty())
			parameters.append(it->body + " __body");
		foreach(auto path, it->pathParams)
			parameters.append(path.second + " " + path.first);
		foreach(auto param, it->parameters)
			parameters.append(param.second + " " + param.first);
		header << parameters.join(", ") << ") const;\n";
	}
	if(!methods.isEmpty())
		header << '\n';
}

void ClassBuilder::writeMemberDeclarations()
{
	for(auto it = classes.constBegin(); it != classes.constEnd(); it++)
		header << "\t" << it.value() << " *_" << it.key() << ";\n";
}

void ClassBuilder::generateFactoryDefinition()
{
	source << "\n" << className << "::Factory::Factory(RestClient *client, const QStringList &parentPath) :\n"
		   << "\tclient(client),\n"
		   << "\tsubPath(parentPath)\n"
		   << "{\n"
		   << "\tsubPath.append(" << className << "::Path);\n"
		   << "}\n";
	writeFactoryDefinitions();
	source << "\n" << className << " *" << className << "::Factory::instance(QObject *parent) const\n"
		   << "{\n"
		   << "\tauto rClass = client->createClass(subPath.join('/'));\n"
		   << "\treturn new " << className << "(rClass, parent);\n"
		   << "}\n";
}

void ClassBuilder::writeFactoryDefinitions()
{
	for(auto it = classes.constBegin(); it != classes.constEnd(); it++) {
		source << "\n" << it.value() << "::Factory " << className << "::Factory::" << it.key() << "() const\n"
			   << "{\n"
			   << "\treturn " << it.value() << "::Factory(client, subPath);\n"
			   << "}\n";
	}
}

void ClassBuilder::writeClassDefinitions()
{
	for(auto it = classes.constBegin(); it != classes.constEnd(); it++){
		source << "\n" << it.value() << " *" << className << "::" << it.key() << "() const\n"
			   << "{\n"
			   << "\treturn _" << it.key() << ";\n"
			   << "}\n";
	}
}

void ClassBuilder::writeMethodDefinitions()
{
	for(auto it = methods.constBegin(); it != methods.constEnd(); it++) {
		source << "\nQtRestClient::GenericRestReply<" << it->returns << ", " << it->except << "> *" << className << "::" << it.key() << "(";
		QStringList parameters;
		if(!it->body.isEmpty())
			parameters.append(it->body + " __body");
		foreach(auto path, it->pathParams)
			parameters.append(path.second + " " + path.first);
		foreach(auto param, it->parameters)
			parameters.append(param.second + " " + param.first);
		source << parameters.join(", ") << ") const\n"
			   << "{\n";

		//create parameters
		auto hasPath = writeMethodPath(it.value());
		source << "\tQVariantHash __params;\n";
		foreach(auto param, it->parameters)
			source << "\t__params.insert(\"" << param.first << "\", " << param.first << ");\n";
		source << "\tHeaderHash __headers;\n";
		for(auto jt = it->headers.constBegin(); jt != it->headers.constEnd(); jt++)
			source << "\t__headers.insert(\"" << jt.key() << "\", \"" << jt.value() << "\");\n";

		//make call
		source << "\n\treturn restClass->call<" << it->returns << ", " << it->except << ">(\"" << it->verb << "\", ";
		if(hasPath) {
			if(!it->url.isEmpty())
				source << "QUrl(__path), ";
			else
				source << "__path, ";
		}
		if(!it->body.isEmpty())
			source << "__body, ";
		source << "__params, __headers);\n";
		source << "}\n";
	}
}

void ClassBuilder::writeMemberDefinitions()
{
	for(auto it = classes.constBegin(); it != classes.constEnd(); it++)
		source << "\t,_" << it.key() << "(new " << it.value() << "(restClass->subClass(" << it.value() << "::Path), this))\n";
}

bool ClassBuilder::writeMethodPath(const MethodInfo &info)
{
	if(!info.path.isEmpty())
		source << "\tQString __path = \"" << info.path << "\";\n";
	else if(!info.url.isEmpty())
		source << "\tQString __path = \"" << info.url << "\";\n";
	else if(!info.pathParams.isEmpty())
		source << "\tQString __path;\n";
	else
		return false;

	foreach(auto param, info.pathParams)
		source << "\t__path.append(QVariant::fromValue(" << param.first << ").toString());\n";
	source << "\n";
	return true;
}



ClassBuilder::MethodInfo::MethodInfo() :
	path(),
	verb("GET"),
	pathParams(),
	parameters(),
	headers(),
	body(),
	returns("QObject*"),
	except("QObject*")
{}
