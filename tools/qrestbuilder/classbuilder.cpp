#include "classbuilder.h"
#include <QJsonArray>

#if !QT_HAS_INCLUDE(<variant>) ||  __cplusplus < 201703L
template <>
const ClassBuilder::Expression &ClassBuilder::get(const ClassBuilder::RestAccess::Method::PathInfoBase &info) {
	Q_ASSERT(!info.isParams);
	return info.path;
}
template <>
const RestBuilder::BaseParam &ClassBuilder::get(const ClassBuilder::RestAccess::Method::PathInfoBase &info) {
	Q_ASSERT(info.isParams);
	return info.pathParams;
}
template <>
ClassBuilder::RestAccess::Method::PathInfoList &ClassBuilder::get(ClassBuilder::RestAccess::Method::PathInfo &info) {
	Q_ASSERT(info.isPath);
	return info.path;
}
template <>
const ClassBuilder::RestAccess::Method::PathInfoList &ClassBuilder::get(const ClassBuilder::RestAccess::Method::PathInfo &info) {
	Q_ASSERT(info.isPath);
	return info.path;
}
template <>
const QString &ClassBuilder::get(const ClassBuilder::RestAccess::Method::PathInfo &info) {
	Q_ASSERT(!info.isPath);
	return info.url;
}

template <>
bool ClassBuilder::is<ClassBuilder::Expression>(const ClassBuilder::RestAccess::Method::PathInfoBase &info) {
	return !info.isParams;
}
template <>
bool ClassBuilder::is<RestBuilder::BaseParam>(const ClassBuilder::RestAccess::Method::PathInfoBase &info) {
	return info.isParams;
}
template <>
bool ClassBuilder::is<ClassBuilder::RestAccess::Method::PathInfoList>(const ClassBuilder::RestAccess::Method::PathInfo &info) {
	return info.isPath;
}
template <>
bool ClassBuilder::is<QString>(const ClassBuilder::RestAccess::Method::PathInfo &info) {
	return !info.isPath;
}
#endif

ClassBuilder::ClassBuilder(QXmlStreamReader &reader) :
	RestBuilder(reader)
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
	data.nspace = readAttrib(QStringLiteral("namespace"));
	data.qmlUri = readAttrib(QStringLiteral("qmlUri"));
	if(isApi) {
		apiData.globalName = readAttrib(QStringLiteral("globalName"));
		if(!apiData.globalName.isEmpty())
			apiData.autoCreate = readAttrib<bool>(QStringLiteral("autoCreate"), true);
	}

	data.includes = {
		{false, QStringLiteral("QtCore/QString")},
		{false, QStringLiteral("QtCore/QStringList")},
		{false, QStringLiteral("QtCore/QScopedPointer")},
		{false, QStringLiteral("QtRestClient/RestClient")},
		{false, QStringLiteral("QtRestClient/RestClass")}
	};
	if(hasQml() && isApi)
		data.includes.append({false, QStringLiteral("QtQml/QQmlParserStatus")});

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
			apiData.apiVersion = QVersionNumber::fromString(readAttrib(QStringLiteral("apiVersion")));
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
	method.postParams = readAttrib<bool>(QStringLiteral("postParams"),
										 method.verb == QStringLiteral("POST") && method.body.isEmpty());
	if(method.postParams && !method.body.isEmpty())
		throwReader(QStringLiteral("You cannot have post params AND a method body at the same time"));

	enum { None, Url, Path } mode = None;
	while(reader.readNextStartElement()) {
		checkError();
		if(reader.name() == QStringLiteral("Url")) {
			if(mode != None)
				throwReader(QStringLiteral("You can only specify a single <Url> element per method, and only if you haven't already used <Path> or <PathParam>"));
			mode = Url;
			method.path = reader.readElementText();
			checkError();
		} else if(reader.name() == QStringLiteral("Path")) {
			if(mode == Url)
				throwReader(QStringLiteral("You cannot specify a <Path> element if you already used <Url>"));
			mode = Path;
			get<RestAccess::Method::PathInfoList>(method.path).append(readExpression());
		} else if(reader.name() == QStringLiteral("PathParam")) {
			if(mode == Url)
				throwReader(QStringLiteral("You cannot specify a <PathParam> element if you already used <Url>"));
			mode = Path;
			get<RestAccess::Method::PathInfoList>(method.path).append(readBaseParam());
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

bool ClassBuilder::hasNs()
{
	return !data.nspace.isEmpty();
}

bool ClassBuilder::hasQml()
{
	return !data.qmlUri.isEmpty();
}

void ClassBuilder::generateClass()
{
	//write header
	writeClassBeginDeclaration();
	writeClassMainDeclaration();
	header << "};\n\n";
	if(hasQml())
		writeQmlDeclaration();
	if(hasNs())
		header << "}\n\n";
	if(hasQml())
		header << "Q_DECLARE_METATYPE(" << nsName(QStringLiteral("Qml") + data.name, data.nspace) << ")\n\n";

	//write source
	writeClassBeginDefinition();
	writeClassMainDefinition();
	if(hasQml())
		writeQmlDefinitions();
	writeStartupCode();
}

void ClassBuilder::generateApi()
{
	//write header
	writeClassBeginDeclaration();
	header << "\tstatic " << data.name << "::Factory factory();\n\n";
	header << "\t" << data.name << "(QObject *parent = nullptr);\n";
	writeClassMainDeclaration();
	header << "\n\tstatic QtRestClient::RestClient *generateClient();\n"
		   << "};\n\n";
	if(hasQml())
		writeQmlDeclaration();
	if(hasNs())
		header << "}\n\n";

	//write source
	writeClassBeginDefinition();
	source << "\n" << data.name << "::Factory " << data.name << "::factory()\n"
		   << "{\n"
		   << "\treturn {generateClient(), {}};\n"
		   << "}\n";
	source << "\n" << data.name << "::" << data.name << "(QObject *parent) :\n"
		   << "\t" << data.name << "{generateClient()->createClass({}), parent}\n"
		   << "{}\n";
	writeClassMainDefinition();
	//write API generation
	if(!apiData.globalName.isEmpty())
		writeGlobalApiGeneration();
	else
		writeLocalApiGeneration();
	if(hasQml())
		writeQmlDefinitions();
	writeStartupCode();
}

void ClassBuilder::writeClassBeginDeclaration()
{
	writeIncludes(data.includes);
	if(hasNs())
		header << "namespace " << data.nspace << " {\n\n";
	header << "class " << data.name << "Private;\n"
		   << "class " << data.name << "PrivateFactory;\n";
	header << "class " << exportedName(data.name, data.exportKey) << " : public " << data.base << "\n"
		   << "{\n"
		   << "\tQ_OBJECT\n\n"
		   << "public:\n";
	writeFactoryDeclaration();
}

void ClassBuilder::writeClassMainDeclaration()
{
	header << "\t" << data.name << "(QtRestClient::RestClass *restClass, QObject *parent);\n"
		   << "\t~" << data.name << "() override;\n\n"
		   << "\tQtRestClient::RestClient *restClient() const;\n"
		   << "\tQtRestClient::RestClass *restClass() const;\n\n";

	for(const auto &subClass : qAsConst(data.classes))
		header << "\t" << subClass.type << " *" << subClass.key << "() const;\n";
	if(!data.classes.isEmpty())
		header << '\n';

	for(const auto &method : qAsConst(data.methods)) {
		header << "\tQtRestClient::GenericRestReply<" << method.returns << ", " << method.except << "> *" << method.name
			   << "(" << writeMethodParams(method, true) << ");\n";
	}
	if(!data.methods.isEmpty())
		header << '\n';

	header << "\tvoid setErrorTranslator(std::function<QString(" << data.except << ", int)> fn);\n\n";
	header << "Q_SIGNALS:\n"
		   << "\tvoid apiError(const QString &errorString, int errorCode, QtRestClient::RestReply::ErrorType errorType);\n\n"
		   << "private:\n"
		   << "\tQScopedPointer<" << data.name << "Private> d;\n";
}

void ClassBuilder::writeClassBeginDefinition()
{
	source << "#include \"" << fileName << ".h\"\n\n";
	if(isApi || hasQml()) {
		source << "#include <QtCore/QCoreApplication>\n"
			   << "#include <QtCore/QTimer>\n";
	}
	if(hasQml()) {
		source << "#include <QtQml/QQmlEngine>\n"
			   << "#include <QtQml/QQmlContext>\n";
	}
	source << "#include <QtCore/QPointer>\n"
		   << "using namespace QtRestClient;\n";
	if(hasNs())
		source << "using namespace " << data.nspace << ";\n";
	if(isApi || classData.path.value.isEmpty())
		source << "\nconst QString " << data.name << "::Path;\n";
	else
		source << "\nconst QString " << data.name << "::Path{" << writeExpression(classData.path, true) << "};\n";
	writePrivateDefinitions();
	writeFactoryDefinition();
}

void ClassBuilder::writeClassMainDefinition()
{
	source << "\n" << data.name << "::" << data.name << "(RestClass *restClass, QObject *parent) :\n"
		   << "\t" << data.base << "(parent),\n"
		   << "\td{new " << data.name << "Private{restClass}}\n"
		   << "{\n"
		   << "\td->restClass->setParent(this);\n"
		   << "}\n\n"
		   << data.name << "::~" << data.name << "() = default;\n";
	source << "\nRestClient *" << data.name << "::restClient() const\n"
		   << "{\n"
		   << "\treturn d->restClass->client();\n"
		   << "}\n";
	source << "\nRestClass *" << data.name << "::restClass() const\n"
		   << "{\n"
		   << "\treturn d->restClass;\n"
		   << "}\n";
	writeClassDefinitions();
	writeMethodDefinitions();
	source << "\nvoid " << data.name << "::setErrorTranslator(std::function<QString(" << data.except << ", int)> fn)\n"
		   << "{\n"
		   << "\td->errorTranslator = std::move(fn);\n"
		   << "}\n";
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

QString ClassBuilder::writeMethodParams(const RestAccess::Method &method, bool asHeader)
{
	QStringList parameters;
	if(!method.body.isEmpty())
		parameters.append(QStringLiteral("const ") + method.body + QStringLiteral(" &__body"));
	// add path parameters
	if(is<RestAccess::Method::PathInfoList>(method.path)) {
		for(const auto &seg : qAsConst(get<RestAccess::Method::PathInfoList>(method.path))) {
			if(is<BaseParam>(seg)) {
				const auto &param = get<BaseParam>(seg);
				parameters.append(writeParamArg(param, asHeader));
			}
		}
	}
	// add normal parameters
	parameters.reserve(parameters.size() + method.params.size());
	for(const auto &param : method.params)
		parameters.append(writeParamArg(param, asHeader));

	return parameters.join(QStringLiteral(", "));
}

void ClassBuilder::writeFactoryDeclaration()
{
	header << "\tstatic const QString Path;\n\n"
		   << "\tclass " << exportedName(QStringLiteral("Factory"), data.exportKey) << "\n"
		   << "\t{\n"
		   << "\t\tQ_DISABLE_COPY(Factory)\n"
		   << "\tpublic:\n"
		   << "\t\tFactory(QtRestClient::RestClient *client, QStringList &&parentPath);\n"
		   << "\t\tFactory(Factory &&other);\n"
		   << "\t\tFactory &operator=(Factory &&other);\n"
		   << "\t\t~Factory();\n\n";
	for(const auto &subClass : qAsConst(data.classes))
		header << "\t\t" << subClass.type << "::Factory " << subClass.key << "() const;\n";
	if(!data.classes.isEmpty())
		header << '\n';
	header << "\t\t" << data.name << " *instance(QObject *parent = nullptr) const;\n\n"
		   << "\tprivate:\n"
		   << "\t\tQScopedPointer<" << data.name << "PrivateFactory> d;\n"
		   << "\t};\n\n";
}

void ClassBuilder::writePrivateDefinitions()
{
	// class
	if(hasNs())
		source << "\nnamespace " << data.nspace << " {\n";
	source << "\nclass " << data.name << "Private\n"
		   << "{\n"
		   << "public:\n"
		   << "\tinline " << data.name << "Private(RestClass *restClass) : \n"
		   << "\t\trestClass{restClass}\n"
		   << "\t{}\n\n"
		   << "\tRestClass *restClass;\n"
		   << "\tstd::function<QString(" << data.except << ", int)> errorTranslator;\n";
	for(const auto &subClass : qAsConst(data.classes))
		source << '\t' << subClass.type << " *" << subClass.key << " = nullptr;\n";
	source << "};\n\n";

	// factory
	source << "class " << data.name << "PrivateFactory\n"
		   << "{\n"
		   << "public:\n"
		   << "\tinline " << data.name << "PrivateFactory(RestClient *client, QStringList &&subPath) :\n"
		   << "\tclient{client},\n"
		   << "\t\tsubPath{std::move(subPath)}\n"
		   << "\t{}\n\n"
		   << "\tRestClient *client;\n"
		   << "\tQStringList subPath;\n"
		   << "};\n\n";

	if(hasNs())
		source << "}\n";
}

void ClassBuilder::writeFactoryDefinition()
{
	source << "\n" << data.name << "::Factory::Factory(RestClient *client, QStringList &&parentPath) :\n"
		   << "\td{new " << data.name << "PrivateFactory{client, std::move(parentPath)}}\n"
		   << "{\n"
		   << "\td->subPath.append(" << data.name << "::Path);\n"
		   << "}\n\n"
		   << data.name << "::Factory::Factory(Factory &&other)\n"
		   << "{\n"
		   << "\td.swap(other.d);\n"
		   << "}\n\n"
		   << data.name << "::Factory &" << data.name << "::Factory::operator=(Factory &&other)\n"
		   << "{\n"
		   << "\td.swap(other.d);\n"
		   << "\treturn *this;\n"
		   << "}\n\n"
		   << data.name << "::Factory::~Factory() = default;\n";
	for(const auto &subClass : qAsConst(data.classes)) {
		source << "\n" << subClass.type << "::Factory " << data.name << "::Factory::" << subClass.key << "() const\n"
			   << "{\n"
			   << "\treturn {d->client, std::move(d->subPath)};\n"
			   << "}\n";
	}
	source << "\n" << data.name << " *" << data.name << "::Factory::instance(QObject *parent) const\n"
		   << "{\n"
		   << "\treturn new " << data.name << "{d->client->createClass(d->subPath.join(QLatin1Char('/'))), parent};\n"
		   << "}\n";
}

void ClassBuilder::writeClassDefinitions()
{
	for(const auto &subClass : qAsConst(data.classes)) {
		source << "\n" << subClass.type << " *" << data.name << "::" << subClass.key << "() const\n"
			   << "{\n"
			   << "\tif(!d->" << subClass.key << ")\n"
			   << "\t\td->" << subClass.key << " = new " << subClass.type << "{d->restClass->subClass(" << subClass.type << "::Path), const_cast<" << data.name << "*>(this)};\n"
			   << "\treturn d->" << subClass.key << ";\n"
			   << "}\n";
	}
}

void ClassBuilder::writeMethodDefinitions()
{
	for(const auto &method : qAsConst(data.methods)) {
		source << "\nQtRestClient::GenericRestReply<" << method.returns << ", " << method.except << "> *" << data.name << "::" << method.name
			   << "(" << writeMethodParams(method, false) << ")\n"
			   << "{\n";

		//create parameters
		auto hasPath = writeMethodPath(method.path);
		source << "\tQVariantHash __params {\n";
		for(const auto &param : method.params)
			source << "\t\t{QStringLiteral(\"" << param.key << "\"), QVariant::fromValue(" << param.key << ")},\n";
		source << "\t};\n\n";
		source << "\tHeaderHash __headers {\n";
		for(const auto &header : method.headers)
			source << "\t\t{\"" << header.key << "\", " << writeExpression(header, false) << "},\n";
		source << "\t};\n\n";

		//make call
		source << "\tauto __reply = d->restClass->call<" << method.returns << ", " << method.except << ">(\"" << method.verb << "\", ";
		if(hasPath)
			source << "__path, ";
		if(!method.body.isEmpty())
			source << "__body, ";
		source << "__params, __headers";
		if(method.postParams)
			source << ", true";
		source << ");\n";

		if(method.except == data.except) {
			source << "\tQPointer<" << data.name << "> __this(this);\n"
				   << "\t__reply->onAllErrors([__this](const QString &__e, int __c, RestReply::ErrorType __t){\n"
				   << "\t\tif(__this)\n"
				   << "\t\t\temit __this->apiError(__e, __c, __t);\n"
				   << "\t}, [__this](" << method.except << " __o, int __c){\n"
				   << "\t\tif(__this && __this->d->errorTranslator)\n"
				   << "\t\t\treturn __this->d->errorTranslator(__o, __c);\n"
				   << "\t\telse\n"
				   << "\t\t\treturn QString{};\n"
				   << "\t});\n";
		}

		source << "\treturn __reply;\n"
			   << "}\n";
	}
}

void ClassBuilder::writeStartupCode()
{
	if(!apiData.autoCreate && !hasQml())
		return;

	source << "\nnamespace {\n\n"
		   << "void __" << data.name << "_app_construct()\n"
		   << "{\n";

	if(apiData.autoCreate)
		source << "\tQTimer::singleShot(0, &" << data.name << "::factory);\n";
	if(hasQml()) {
		auto uriParts = data.qmlUri.split(QLatin1Char(' '), QString::SkipEmptyParts);
		auto uriPath = uriParts.takeFirst();
		auto uriVersion = QVersionNumber::fromString(uriParts.join(QLatin1Char(' ')));
		if(uriVersion.isNull())
			uriVersion = {1,0};

		if(isApi) {
			source << "\tqmlRegisterType<Qml" << data.name << ">(\""
				   << uriPath << "\", " << uriVersion.majorVersion() << ", " << uriVersion.minorVersion()
				   << ", \"" << data.name << "\");\n";
		} else {
			source << "\tqRegisterMetaType<Qml" << data.name << ">();\n"
				   << "\tqmlRegisterUncreatableType<Qml" << data.name << ">(\""
				   << uriPath << "\", " << uriVersion.majorVersion() << ", " << uriVersion.minorVersion()
				   << ", \"" << data.name << "\", QStringLiteral(\"Q_GADGETs cannot be created from QML\"));\n";
		}
	}

	source << "}\n\n"
		   << "}\n"
		   << "Q_COREAPP_STARTUP_FUNCTION(__" << data.name << "_app_construct)\n";
}

void ClassBuilder::writeLocalApiGeneration()
{
	source << "\nRestClient *" << data.name << "::generateClient()\n"
		   << "{\n"
		   << "\tstatic QPointer<RestClient> client {[](){\n"
		   << "\t\tauto ";
	writeApiCreation();
	source << "\t\treturn client;\n"
		   << "\t}()};\n"
		   << "\treturn client;\n"
		   << "}\n";
}

void ClassBuilder::writeGlobalApiGeneration()
{
	source << "\nRestClient *" << data.name << "::generateClient()\n"
		   << "{\n"
		   << "\tauto client = apiClient(QStringLiteral(\"" << apiData.globalName << "\"));\n"
		   << "\tif(!client) {\n"
		   << "\t\t";
	writeApiCreation();
	source << "\t\taddGlobalApi(QStringLiteral(\"" << apiData.globalName << "\"), client);\n"
		   << "\t}\n"
		   << "\treturn client;\n"
		   << "}\n";
}

void ClassBuilder::writeApiCreation()
{
	source << "client = new RestClient(QCoreApplication::instance());\n"
		   << "\t\tclient->setBaseUrl(QUrl{" << writeExpression(apiData.baseUrl, true) << "});\n";
	if(!apiData.apiVersion.isNull()) {
		QStringList args;
		args.reserve(apiData.apiVersion.segmentCount());
		for(const auto &segment : apiData.apiVersion.segments())
			args.append(QString::number(segment));
		source << "\t\tclient->setApiVersion(QVersionNumber{" << args.join(QStringLiteral(", ")) << "});\n";
	}
	for(const auto &header : qAsConst(apiData.headers))
		source << "\t\tclient->addGlobalHeader(\"" << header.key << "\", " << writeExpression(header, false) << ");\n";
	for(const auto &param : qAsConst(apiData.params))
		source << "\t\tclient->addGlobalParameter(QStringLiteral(\"" << param.key << "\"), " << writeExpression(param, true) << ");\n";
}

void ClassBuilder::writeQmlDeclaration()
{
	if(isApi) {
		header << "class Qml" << data.name << " : public QObject, public QQmlParserStatus\n"
			   << "{\n"
			   << "\tQ_OBJECT\n"
			   << "\tQ_INTERFACES(QQmlParserStatus)\n\n";
	} else {
		header << "class Qml" << data.name << "\n"
			   << "{\n"
			   << "\tQ_GADGET\n\n";
	}
	for(const auto &subClass : qAsConst(data.classes))
		header << "\tQ_PROPERTY(" << nsInject(subClass.type, QStringLiteral("Qml")) << " " << subClass.key << " READ " << subClass.key << " CONSTANT)\n";
	if(!data.classes.isEmpty())
		header << '\n';

	if(isApi) {
		header << "public:\n"
			   << "\tQml" << data.name << "(QObject *parent = nullptr);\n\n"
			   << "\tvoid classBegin() override;\n"
			   << "\tvoid componentComplete() override;\n\n";
	} else {
		header << "public:\n"
			   << "\tQml" << data.name << "(" << data.name << "* instance = nullptr, void *engine = nullptr);\n\n";
	}

	for(const auto &subClass : qAsConst(data.classes))
		header << "\t" << nsInject(subClass.type, QStringLiteral("Qml")) << " " << subClass.key << "() const;\n";
	if(!data.classes.isEmpty())
		header << '\n';

	for(const auto &method : qAsConst(data.methods)) {
		header << "\tQ_INVOKABLE QtRestClient::QmlGenericRestReply *" << method.name
			   << "(" << writeMethodParams(method, true) << ");\n";
	}
	if(!data.methods.isEmpty())
		header << '\n';

	header << "private:\n"
		   << "\tQPointer<" << data.name << "> _d;\n"
		   << "\tvoid *_engine;\n"
		   << "};\n\n";
}

void ClassBuilder::writeQmlDefinitions()
{
	if(isApi) {
		source << "\nQml" << data.name << "::Qml" << data.name << "(QObject *parent) :\n"
			   << "\tQObject{parent},\n"
			   << "\t_d{new " << data.name << "{this}},\n"
			   << "\t_engine{nullptr}\n"
			   << "{\n"
			   << "}\n"
			   << "\nvoid Qml" << data.name << "::classBegin() {}\n"
			   << "\nvoid Qml" << data.name << "::componentComplete()\n"
			   << "{\n"
			   << "\t_engine = QQmlEngine::contextForObject(this)->engine();\n"
			   << "\tQ_ASSERT_X(_engine, Q_FUNC_INFO, \"Generated QML-APIs can only be created for a QML context with a JSEngine\");\n"
			   << "}\n";
	} else {
		source << "\nQml" << data.name << "::Qml" << data.name << "(" << data.name << " *instance, void *engine) :\n"
			   << "\t_d{instance},\n"
			   << "\t_engine{engine}\n"
			   << "{}\n";
	}

	for(const auto &subClass : qAsConst(data.classes)) {
		source << "\n" << nsInject(subClass.type, QStringLiteral("Qml")) << " Qml" << data.name << "::" << subClass.key << "() const\n"
			   << "{\n"
			   << "\treturn {_d ? _d->"  << subClass.key << "() : nullptr, _engine};\n"
			   << "}\n";
	}

	for(const auto &method : qAsConst(data.methods)) {
		source << "\nQtRestClient::QmlGenericRestReply *Qml" << data.name << "::" << method.name
			   << "(" << writeMethodParams(method, false) << ")\n"
			   << "{\n"
			   << "\tif(!_d)\n"
			   << "\t\treturn nullptr;\n"
			   << "\tauto reply = _d->" << method.name << "(";

		QStringList parameters;
		if(!method.body.isEmpty())
			parameters.append(QStringLiteral("__body"));
		// add path parameters
		if(is<RestAccess::Method::PathInfoList>(method.path)) {
			for(const auto &seg : qAsConst(get<RestAccess::Method::PathInfoList>(method.path))) {
				if(is<BaseParam>(seg))
					parameters.append(get<BaseParam>(seg).key);
			}
		}
		// add normal parameters
		parameters.reserve(parameters.size() + method.params.size());
		for(const auto &param : method.params)
			parameters.append(param.key);

		source << parameters.join(QStringLiteral(", "));

		source << ");\n"
			   << "\tif(!reply)\n"
			   << "\t\treturn nullptr;\n"
			   << "\treturn reinterpret_cast<QtRestClient::QmlGenericRestReply*>(QMetaType::metaObjectForType(QMetaType::type(\"QtRestClient::QmlGenericRestReply*\"))\n"
			   << "\t\t->newInstance(Q_ARG(QJsonSerializer*, _d->restClient()->serializer()),\n"
			   << "\t\t\tQ_ARG(QJSEngine*, reinterpret_cast<QJSEngine*>(_engine)),\n"
			   << "\t\t\tQ_ARG(int, QMetaType::type(\"" << method.returns << "\")),\n"
			   << "\t\t\tQ_ARG(int, QMetaType::type(\"" << method.except << "\")),\n"
			   << "\t\t\tQ_ARG(QtRestClient::RestReply*, reply)));\n"
			   << "}\n";
	}
}

bool ClassBuilder::writeMethodPath(const RestAccess::Method::PathInfo &info)
{
	if(is<RestAccess::Method::PathInfoList>(info)) {
		const auto &pathList = get<RestAccess::Method::PathInfoList>(info);
		if(pathList.isEmpty())
			return false;
		else {
			source << "\tQString __path = QStringList {\n";
			for(const auto &seg : pathList) {
				if(is<BaseParam>(seg))
					source << "\t\tQVariant::fromValue(" << get<BaseParam>(seg).key << ").toString(),\n";
				else if(is<Expression>(seg))
					source << "\t\t" << writeExpression(get<Expression>(seg), true) << ",\n";
			}
			source << "\t}.join(QLatin1Char('/'));\n\n";
		}
	} else if(is<QString>(info))
		source << "\tQUrl __path {QStringLiteral(\"" << get<QString>(info) << "\")};\n\n";
	else
		return false;
	return true;
}
