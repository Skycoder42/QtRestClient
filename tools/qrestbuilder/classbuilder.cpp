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

	readClasses();
	readMethods();
	auto parent = root["parent"].toString("QObject");

	//write header
	writeClassBeginDeclaration(parent);
	writeClassMainDeclaration();
	header << "};\n\n";

	//write source
	writeClassBeginDefinition();
	writeClassMainDefinition(parent);
}

void ClassBuilder::generateApi()
{
	qInfo() << "generating api:" << className;

	readClasses();
	readMethods();
	auto parent = root["parent"].toString("QObject");

	//write header
	writeClassBeginDeclaration(parent);
	header << "\tstatic " << className << "::Factory factory();\n";
	header << "\t" << className << "(QObject *parent = nullptr);\n";
	writeClassMainDeclaration();
	header << "\n\tstatic QtRestClient::RestClient *generateClient();\n"
		   << "};\n\n";

	//write source
	writeClassBeginDefinition();
	source << "\n" << className << "::Factory " << className << "::factory()\n"
		   << "{\n"
		   << "\treturn " << className << "::Factory(generateClient(), {});\n"
		   << "}\n";
	source << "\n" << className << "::" << className << "(QObject *parent) :\n"
		   << "\t" << className << "(generateClient()->createClass(QString()), parent)\n"
		   << "{}\n";
	writeClassMainDefinition(parent);

	//write API generation
	auto globalName = root["globalName"].toString();
	if(!globalName.isEmpty())
		writeGlobalApiGeneration(globalName);
	else
		writeLocalApiGeneration();
}

void ClassBuilder::writeClassBeginDeclaration(const QString &parent)
{
	auto includes = readIncludes();
	includes.append("QtRestClient/restclient.h");
	includes.append("QtRestClient/restclass.h");
	includes.append("QtCore/qstring.h");
	includes.append("QtCore/qstringlist.h");

	writeIncludes(header, includes);
	header << "class " << exportedClassName << " : public " << parent << "\n"
		   << "{\n"
		   << "\tQ_OBJECT\n\n"
		   << "public:\n";
	generateFactoryDeclaration();
}

void ClassBuilder::writeClassMainDeclaration()
{
	header << "\t" << className << "(QtRestClient::RestClass *restClass, QObject *parent);\n\n"
		   << "\tQtRestClient::RestClient *restClient() const;\n"
		   << "\tQtRestClient::RestClass *restClass() const;\n\n";
	writeClassDeclarations();
	writeMethodDeclarations();
	header << "private:\n"
		   << "\tQtRestClient::RestClass *_restClass;\n";
	writeMemberDeclarations();
}

void ClassBuilder::writeClassBeginDefinition()
{
	source << "#include \"" << fileName << ".h\"\n\n"
		   << "#include <QtCore/qcoreapplication.h>\n"
		   << "#include <QtCore/qtimer.h>\n"
		   << "using namespace QtRestClient;\n\n"
		   << "const QString " << className << "::Path(\"" << root["path"].toString() << "\");\n";
	generateFactoryDefinition();
}

void ClassBuilder::writeClassMainDefinition(const QString &parent)
{
	source << "\n" << className << "::" << className << "(RestClass *restClass, QObject *parent) :\n"
		   << "\t" << parent << "(parent)\n"
		   << "\t,_restClass(restClass)\n";
	writeMemberDefinitions();
	source << "{\n"
		   << "\t_restClass->setParent(this);\n"
		   << "}\n";
	source << "\nRestClient *" << className << "::restClient() const\n"
		   << "{\n"
		   << "\treturn _restClass->client();\n"
		   << "}\n";
	source << "\nRestClass *" << className << "::restClass() const\n"
		   << "{\n"
		   << "\treturn _restClass;\n"
		   << "}\n";
	writeClassDefinitions();
	writeMethodDefinitions();
}

void ClassBuilder::readClasses()
{
	auto cls = root["classes"].toObject();
	for(auto it = cls.constBegin(); it != cls.constEnd(); it++)
		classes.insert(it.key(), it.value().toString());
}

void ClassBuilder::readMethods()
{
	auto defExcept = root["except"].toString("QObject*");
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
			auto param = value.toString().split(';');
			if(param.size() != 2)
				throw QStringLiteral("Element in pathParams must be ,-seperated!");
			info.pathParams.append({param[0], param[1]});
		}
		foreach(auto value, obj["parameters"].toArray()) {
			auto param = value.toString().split(';');
			if(param.size() != 2)
				throw QStringLiteral("Element in parameters must be ,-seperated!");
			info.parameters.append({param[0], param[1]});
		}
		auto headers = obj["headers"].toObject();
		for(auto jt = headers.constBegin(); jt != headers.constEnd(); jt++)
			info.headers.insert(jt.key(), jt.value().toString());
		info.body = obj["body"].toString(info.body);
		info.returns = obj["returns"].toString(info.returns);
		info.except = obj["except"].toString(defExcept);

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
		header << "\tQtRestClient::GenericRestReply<" << it->returns << ", " << it->except << "> *" << it.key() << "(";
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
		source << "\n\treturn _restClass->call<" << it->returns << ", " << it->except << ">(\"" << it->verb << "\", ";
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
		source << "\t,_" << it.key() << "(new " << it.value() << "(_restClass->subClass(" << it.value() << "::Path), this))\n";
}

void ClassBuilder::writeLocalApiGeneration()
{
	source << "\nRestClient *" << className << "::generateClient()\n"
		   << "{\n"
		   << "\tstatic QPointer<RestClient> client = nullptr;\n"
		   << "\tif(!client) {\n";
	writeApiCreation();
	source << "\t}\n"
		   << "\treturn client;\n"
		   << "}\n";
}

void ClassBuilder::writeGlobalApiGeneration(const QString &globalName)
{
	source << "\nRestClient *" << className << "::generateClient()\n"
		   << "{\n"
		   << "\tauto client = apiClient(\"" << globalName << "\");\n"
		   << "\tif(!client) {\n";
	writeApiCreation();
	source << "\t\taddGlobalApi(\"" << globalName << "\", client);\n"
		   << "\t}\n"
		   << "\treturn client;\n"
		   << "}\n";

	if(root["autoCreate"].toBool(true)) {
		source << "\nstatic void __" << className << "_app_construct()\n"
			   << "{\n"
			   << "\tQTimer::singleShot(0, &" << className << "::factory);\n"
			   << "}\n"
			   << "Q_COREAPP_STARTUP_FUNCTION(__" << className << "_app_construct)\n";
	}
}

void ClassBuilder::writeApiCreation()
{
	source << "\t\tclient = new RestClient(QCoreApplication::instance());\n"
		   << "\t\tclient->setBaseUrl(QUrl(\"" << root["baseUrl"].toString() << "\"));\n";
	auto version = root["apiVersion"].toString();
	if(!version.isEmpty())
		source << "\t\tclient->setApiVersion(QVersionNumber::fromString(\"" << version << "\"));\n";
	auto headers = root["headers"].toObject();
	for(auto it = headers.constBegin(); it != headers.constEnd(); it++)
		source << "\t\tclient->addGlobalHeader(\"" << it.key() << "\", \"" << it.value().toString() << "\");\n";
	auto parameters = root["parameters"].toObject();
	for(auto it = parameters.constBegin(); it != parameters.constEnd(); it++)
		source << "\t\tclient->addGlobalParameter(\"" << it.key() << "\", \"" << it.value().toString() << "\");\n";
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
