#include "classbuilder.h"
#include <QJsonArray>

ClassBuilder::ClassBuilder(QXmlStreamReader &reader, QObject *parent) :
	RestBuilder(reader, parent)
{}

bool ClassBuilder::canReadType(const QString &type)
{
	return type == QStringLiteral("RestApi") ||
			type == QStringLiteral("RestClass");
}

void ClassBuilder::build()
{
	readData();
	if(isApi)
		generateApi();
	else
		generateClass();
}

void ClassBuilder::readData()
{
	if(reader.name() == QStringLiteral("RestApi"))
		isApi = true;
	else if(reader.name() == QStringLiteral("RestClass"))
		isApi = false;
	else
		Q_UNREACHABLE();

	// read common data
	data.name = readAttrib(QStringLiteral("name"), {}, true);
	data.base = readAttrib(QStringLiteral("base"), QStringLiteral("QObject"));
	data.except = readAttrib(QStringLiteral("except"), QStringLiteral("QObject*"));
	data.exportKey = readAttrib(QStringLiteral("export"));
	if(isApi) {
		apiData.globalName = readAttrib(QStringLiteral("globalName"));
		apiData.autoCreate = readAttrib<bool>(QStringLiteral("autoCreate"), !apiData.globalName.isEmpty());
	}

	data.includes = {
		{false, QStringLiteral("QtCore/QString")},
		{false, QStringLiteral("QtCore/QStringList")},
		{false, QStringLiteral("QtCore/QScopedPointer")},
		{false, QStringLiteral("QtRestClient/RestClient")},
		{false, QStringLiteral("QtRestClient/RestClass")}
	};

	// read content
	while(reader.readNextStartElement()) {
		checkError();
		if(reader.name() == QStringLiteral("Include"))
			data.includes.append(readInclude());
		else if(reader.name() == QStringLiteral("Class"))
			data.classes.append(readClass());
		else if(reader.name() == QStringLiteral("Method"))
			data.methods.append(readMethod());
		else if(isApi && reader.name() == QStringLiteral("BaseUrl")) {
			apiData.apiVersion = readAttrib<QVersionNumber>(QStringLiteral("apiVersion"));
			apiData.baseUrl = readExpression();
		} else if(isApi && reader.name() == QStringLiteral("Parameter"))
			apiData.params.append(readFixedParam());
		else if(isApi && reader.name() == QStringLiteral("Header"))
			apiData.headers.append(readFixedParam());
		else if(!isApi && reader.name() == QStringLiteral("Path"))
			classData.path = readExpression();
		else
			throwChild();
	}
	checkError();
}

ClassBuilder::Expression ClassBuilder::readExpression()
{
	Expression expr;
	expr.expr = readAttrib<bool>(QStringLiteral("expr"), false);
	expr.value = reader.readElementText();
	checkError();
	return expr;
}

ClassBuilder::FixedParam ClassBuilder::readFixedParam()
{
	FixedParam param;
	param.key = readAttrib(QStringLiteral("key"), {}, true);
	param.expr = readAttrib<bool>(QStringLiteral("expr"), false);
	param.value = reader.readElementText();
	checkError();
	return param;
}

ClassBuilder::RestAccess::Class ClassBuilder::readClass()
{
	RestAccess::Class restClass;
	restClass.key = readAttrib(QStringLiteral("key"), {}, true);
	restClass.type = readAttrib(QStringLiteral("type"), {}, true);
	if(reader.readNextStartElement())
		throwChild();
	checkError();
	return restClass;
}

ClassBuilder::RestAccess::Method ClassBuilder::readMethod()
{
	RestAccess::Method method;
	method.name = readAttrib(QStringLiteral("name"), {}, true);
	method.verb = readAttrib(QStringLiteral("verb"), QStringLiteral("GET"));
	method.body = readAttrib(QStringLiteral("body"));
	method.returns = readAttrib(QStringLiteral("returns"), QStringLiteral("void"));
	method.except = readAttrib(QStringLiteral("except"), data.except);
	method.postParams = readAttrib<bool>(QStringLiteral("postParams"), method.verb == QStringLiteral("POST"));

	enum { None, Url, Path } mode = None;
	while(reader.readNextStartElement()) {
		checkError();
		if(reader.name() == QStringLiteral("Url")) {
			if(mode != None)
				throwReader(tr("You cannot specify a single <Url> element per method, and only if you haven't already used <Path> or <PathParam>"));
			mode = Url;
			method.path = reader.readElementText();
			checkError();
		} else if(reader.name() == QStringLiteral("Path")) {
			if(mode == Url)
				throwReader(tr("You cannot specify a <Path> element if you already used <Url>"));
			mode = Path;
			std::get<RestAccess::Method::PathInfoBase>(method.path).append(readExpression());
		} else if(reader.name() == QStringLiteral("PathParam")) {
			if(mode == Url)
				throwReader(tr("You cannot specify a <PathParam> element if you already used <Url>"));
			mode = Path;
			std::get<RestAccess::Method::PathInfoBase>(method.path).append(readBaseParam());
		} else if(reader.name() == QStringLiteral("Param"))
			method.params.append(readBaseParam());
		else if(reader.name() == QStringLiteral("Header"))
			method.headers.append(readFixedParam());
		else
			throwChild();
	}
	checkError();
	return method;
}

void ClassBuilder::generateClass()
{
	//write header
	writeClassBeginDeclaration();
	writeClassMainDeclaration();
	header << "};\n\n";

	//write source
	writeClassBeginDefinition();
	writeClassMainDefinition();
}

void ClassBuilder::generateApi()
{
//	readClasses();
//	readMethods();
//	auto parent = root[QStringLiteral("parent")].toString(QStringLiteral("QObject"));

//	//write header
//	writeClassBeginDeclaration(parent);
//	header << "\tstatic " << className << "::Factory factory();\n";
//	header << "\t" << className << "(QObject *parent = nullptr);\n";
//	writeClassMainDeclaration();
//	header << "\n\tstatic QtRestClient::RestClient *generateClient();\n"
//		   << "};\n\n";

//	//write source
//	writeClassBeginDefinition();
//	source << "\n" << className << "::Factory " << className << "::factory()\n"
//		   << "{\n"
//		   << "\treturn " << className << "::Factory(generateClient(), {});\n"
//		   << "}\n";
//	source << "\n" << className << "::" << className << "(QObject *parent) :\n"
//		   << "\t" << className << "(generateClient()->createClass(QString()), parent)\n"
//		   << "{}\n";
//	writeClassMainDefinition(parent);

//	//write API generation
//	auto globalName = root[QStringLiteral("globalName")].toString();
//	if(!globalName.isEmpty())
//		writeGlobalApiGeneration(globalName);
//	else
//		writeLocalApiGeneration();
}

void ClassBuilder::writeClassBeginDeclaration()
{
	writeIncludes(data.includes);
	header << "class " << data.name << "Private;\n"
		   << "class " << data.name << "Private::Factory;\n"
		   << "class " << exportedName(data.name, data.exportKey) << " : public " << data.base << "\n"
		   << "{\n"
		   << "\tQ_OBJECT\n\n"
		   << "public:\n";
	generateFactoryDeclaration();
}

void ClassBuilder::writeClassMainDeclaration()
{
	header << "\t" << data.name << "(QtRestClient::RestClass *restClass, QObject *parent);\n\n"
		   << "\tQtRestClient::RestClient *restClient() const;\n"
		   << "\tQtRestClient::RestClass *restClass() const;\n\n";
	writeClassDeclarations();
	writeMethodDeclarations();
	header << "\tvoid setErrorTranslator(const std::function<QString(" << data.except << ", int)> &fn);\n\n"
		   << "Q_SIGNALS:\n"
		   << "\tvoid apiError(const QString &errorString, int errorCode, QtRestClient::RestReply::ErrorType errorType);\n\n"
		   << "private:\n"
		   << "\tQScopedPointer<" << data.name << "Private> d;\n";
	writeMemberDeclarations();
}

void ClassBuilder::writeClassBeginDefinition()
{
	source << "#include \"" << fileName << ".h\"\n\n"
		   << "#include <QtCore/QCoreApplication>\n"
		   << "#include <QtCore/QTimer>\n"
		   << "#include <QtCore/QPointer>\n"
		   << "using namespace QtRestClient;\n\n"
		   << "const QString " << data.name << "::Path{" << writeExpression(classData.path, true) << "};\n";
	writePrivateDefinitions();
	generateFactoryDefinition();
}

void ClassBuilder::writeClassMainDefinition()
{
	source << "\n" << data.name << "::" << data.name << "(RestClass *restClass, QObject *parent) :\n"
		   << "\t" << data.base << "(parent),\n"
		   << "\td{new " << data.name << "Private{restClass}}\n"
		   << "{\n"
		   << "\td->restClass->setParent(this);\n"
		   << "}\n";
	source << "\nRestClient *" << data.name << "::restClient() const\n"
		   << "{\n"
		   << "\treturn d->restClass->client();\n"
		   << "}\n";
	source << "\nRestClass *" << data.name << "::restClass() const\n"
		   << "{\n"
		   << "\treturn d->restClass;\n"
		   << "}\n";
	writeClassDefinitions();
//	writeMethodDefinitions();
//	source << "\nvoid " << className << "::setErrorTranslator(const std::function<QString(" << defaultExcept << ", int)> &fn)\n"
//		   << "{\n"
//		   << "\t_errorTranslator = fn;\n"
	//		   << "}\n";
}

QString ClassBuilder::writeExpression(const ClassBuilder::Expression &expression, bool asString)
{
	if(expression.expr)
		return expression.value;
	else if(asString)
		return QStringLiteral("QStringLiteral(\"") + expression.value + QStringLiteral("\")");
	else
		return QLatin1Char('"') + expression.value + QLatin1Char('"');
}

void ClassBuilder::generateFactoryDeclaration()
{
	//TODO make factory an "rvalue" class
	header << "\tstatic const QString Path;\n\n"
		   << "\tclass " << exportedName(QStringLiteral("Factory"), data.exportKey) << "\n"
		   << "\t{\n"
		   << "\tpublic:\n"
		   << "\t\tFactory(QtRestClient::RestClient *client, QStringList parentPath);\n\n";
	writeFactoryDeclarations();
	header << "\t\t" << data.name << " *instance(QObject *parent = nullptr) const;\n\n"
		   << "\tprivate:\n"
		   << "\t\tQScopedPointer<" << data.name << "Private::Factory> d;\n"
		   << "\t};\n\n";
}

void ClassBuilder::writeFactoryDeclarations()
{
	for(const auto &subClass : qAsConst(data.classes))
		header << "\t\t" << subClass.type << "::Factory " << subClass.key << "() const;\n";
	if(!data.classes.isEmpty())
		header << '\n';
}

void ClassBuilder::writeClassDeclarations()
{
	for(const auto &subClass : qAsConst(data.classes))
		header << "\t" << subClass.type << " *" << subClass.key << "() const;\n";
	if(!data.classes.isEmpty())
		header << '\n';
}

void ClassBuilder::writeMethodDeclarations()
{
	//TODO check param as value or const ref EVERYWHERE
	for(const auto &method : qAsConst(data.methods)) {
		header << "\tQtRestClient::GenericRestReply<" << method.returns << ", " << method.except << "> *" << method.name << "(";
		QStringList parameters;
		if(!method.body.isEmpty())
			parameters.append(method.body + QStringLiteral(" __body"));
		// add path parameters
		if(std::holds_alternative<RestAccess::Method::PathInfoBase>(method.path)) {
			for(const auto &seg : qAsConst(std::get<RestAccess::Method::PathInfoBase>(method.path))) {
				if(std::holds_alternative<BaseParam>(seg))
					parameters.append(writeParamArg(std::get<BaseParam>(seg)));
			}
		}
		// add normal parameters
		for(const auto &param : method.params)
			parameters.append(writeParamArg(param));
		header << parameters.join(QStringLiteral(", ")) << ");\n";
	}
	if(!data.methods.isEmpty())
		header << '\n';
}

void ClassBuilder::writeMemberDeclarations()
{
	for(const auto &subClass : qAsConst(data.classes))
		header << '\t' << subClass.type << " *" << subClass.key << " = nullptr;\n";
}

void ClassBuilder::writePrivateDefinitions()
{
	source << "\nclass " << data.name << "Private\n"
		   << "{\n"
		   << "public:\n";
	source << "\tclass Factory\n"
		   << "\t{\n"
		   << "\tpublic:\n"
		   << "\t\tinline Factory(RestClient *client, QStringList &&subPath) :\n"
		   << "\t\t\tclient{client},\n"
		   << "\t\t\tsubPath{std::move(subPath)}\n"
		   << "\t\t{}\n\n"
		   << "\t\tRestClient *client;\n"
		   << "\t\tQStringList subPath;\n"
		   << "\t};\n\n";
	source << "\tinline " << data.name << "Private(RestClass *restClass) : \n"
		   << "\t\trestClass{restClass}\n"
		   << "\t{}\n\n"
		   << "\tRestClass *restClass;\n"
		   << "\tstd::function<QString(QObject*, int)> errorTranslator;\n";
	writeMemberDeclarations();
	source << "};\n";
}

void ClassBuilder::generateFactoryDefinition()
{
	source << "\n" << data.name << "::Factory::Factory(RestClient *client, QStringList parentPath) :\n"
		   << "\td{new " << data.name << "Private::Factory{client, std::move(parentPath)}},\n"
		   << "{\n"
		   << "\td->subPath.append(" << data.name << "::Path);\n"
		   << "}\n";
	writeFactoryDefinitions();
	source << "\n" << data.name << " *" << data.name << "::Factory::instance(QObject *parent) const\n"
		   << "{\n"
		   << "\treturn new " << data.name << "{d->client->createClass(d->subPath.join(QLatin1Char('/'))), parent};\n"
		   << "}\n";
}

void ClassBuilder::writeFactoryDefinitions()
{
	for(const auto &subClass : qAsConst(data.classes)) {
		source << "\n" << subClass.type << "::Factory " << data.name << "::Factory::" << subClass.key << "() const\n"
			   << "{\n"
			   << "\treturn {d->client, d->subPath};\n"
			   << "}\n";
	}
}

void ClassBuilder::writeClassDefinitions()
{
	for(const auto &subClass : qAsConst(data.classes)) {
		source << "\n" << subClass.type << " *" << data.name << "::" << subClass.key << "() const\n"
			   << "{\n"
			   << "\tif(!d->" << subClass.key << ")\n"
			   << "\t\td->" << subClass.key << " = d->restClass->subClass(" << subClass.type << "::Path, this);\n"
			   << "\treturn d->" << subClass.key << ";\n"
			   << "}\n";
	}
}

void ClassBuilder::writeMethodDefinitions()
{
//	for(auto it = methods.constBegin(); it != methods.constEnd(); it++) {
//		source << "\nQtRestClient::GenericRestReply<" << it->returns << ", " << it->except << "> *" << className << "::" << it.key() << "(";
//		QStringList parameters;
//		if(!it->body.isEmpty())
//			parameters.append(it->body + QStringLiteral(" __body"));
//		for(const auto &path : it->pathParams)
//			parameters.append(path.write(false));
//		for(const auto &param : it->parameters)
//			parameters.append(param.write(false));
//		source << parameters.join(QStringLiteral(", ")) << ")\n"
//			   << "{\n";

//		//create parameters
//		auto hasPath = writeMethodPath(it.value());
//		source << "\tQVariantHash __params;\n";
//		for(const auto &param : it->parameters)
//			source << "\t__params.insert(QStringLiteral(\"" << param.name << "\"), " << param.name << ");\n";
//		source << "\tHeaderHash __headers;\n";
//		for(auto jt = it->headers.constBegin(); jt != it->headers.constEnd(); jt++)
//			source << "\t__headers.insert(\"" << jt.key() << "\", " << expr(jt.value(), false) << ");\n";

//		//make call
//		source << "\n\tauto __reply = _restClass->call<" << it->returns << ", " << it->except << ">(" << expr(it->verb, false) << ", ";
//		if(hasPath) {
//			if(!it->url.isEmpty())
//				source << "QUrl(__path), ";
//			else
//				source << "__path, ";
//		}
//		if(!it->body.isEmpty())
//			source << "__body, ";
//		source << "__params, __headers);\n";

//		if(it->except == defaultExcept) {
//			source << "\tQPointer<" << className << "> __this(this);\n"
//				   << "\t__reply->onAllErrors([__this](QString __e, int __c, RestReply::ErrorType __t){\n"
//				   << "\t\tif(__this)\n"
//				   << "\t\t\temit __this->apiError(__e, __c, __t);\n"
//				   << "\t}, [__this](" << it->except << " __o, int __c){\n"
//				   << "\t\tif(__this && __this->_errorTranslator)\n"
//				   << "\t\t\treturn __this->_errorTranslator(__o, __c);\n"
//				   << "\t\telse\n"
//				   << "\t\t\treturn QString();\n"
//				   << "\t});\n";
//		}

//		source << "\treturn __reply;\n"
//			   << "}\n";
//	}
}

void ClassBuilder::writeLocalApiGeneration()
{
//	source << "\nRestClient *" << className << "::generateClient()\n"
//		   << "{\n"
//		   << "\tstatic QPointer<RestClient> client = nullptr;\n"
//		   << "\tif(!client) {\n";
//	writeApiCreation();
//	source << "\t}\n"
//		   << "\treturn client;\n"
//		   << "}\n";
}

void ClassBuilder::writeGlobalApiGeneration(const QString &globalName)
{
//	auto golbalExpr = expr(globalName, true);
//	source << "\nRestClient *" << className << "::generateClient()\n"
//		   << "{\n"
//		   << "\tauto client = apiClient(" << golbalExpr << ");\n"
//		   << "\tif(!client) {\n";
//	writeApiCreation();
//	source << "\t\taddGlobalApi(" << golbalExpr << ", client);\n"
//		   << "\t}\n"
//		   << "\treturn client;\n"
//		   << "}\n";

//	if(root[QStringLiteral("autoCreate")].toBool(true)) {
//		source << "\nstatic void __" << className << "_app_construct()\n"
//			   << "{\n"
//			   << "\tQTimer::singleShot(0, &" << className << "::factory);\n"
//			   << "}\n"
//			   << "Q_COREAPP_STARTUP_FUNCTION(__" << className << "_app_construct)\n";
//	}
}

void ClassBuilder::writeApiCreation()
{
//	source << "\t\tclient = new RestClient(QCoreApplication::instance());\n"
//		   << "\t\tclient->setBaseUrl(QUrl(" << expr(root[QStringLiteral("baseUrl")].toString(), true) << "));\n";
//	auto version = root[QStringLiteral("apiVersion")].toString();
//	if(!version.isEmpty())
//		source << "\t\tclient->setApiVersion(QVersionNumber::fromString(" << expr(version, true) << "));\n";
//	auto headers = root[QStringLiteral("headers")].toObject();
//	for(auto it = headers.constBegin(); it != headers.constEnd(); it++)
//		source << "\t\tclient->addGlobalHeader(\"" << it.key() << "\", " << expr(it.value().toString(), false) << ");\n";
//	auto parameters = root[QStringLiteral("parameters")].toObject();
//	for(auto it = parameters.constBegin(); it != parameters.constEnd(); it++)
//		source << "\t\tclient->addGlobalParameter(QStringLiteral(\"" << it.key() << "\"), " << expr(it.value().toString(), true) << ");\n";
}

bool ClassBuilder::writeMethodPath()
{
//	if(!info.path.isEmpty())
//		source << "\tQString __path = " << expr(info.path, true) << ";\n";
//	else if(!info.url.isEmpty())
//		source << "\tQString __path = " << expr(info.url, true) << ";\n";
//	else if(!info.pathParams.isEmpty())
//		source << "\tQString __path;\n";
//	else
//		return false;

//	for(const auto &param : info.pathParams)
//		source << "\t__path.append(QVariant::fromValue(" << param.name << ").toString());\n";
//	source << "\n";
//	return true;
}
