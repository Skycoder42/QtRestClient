#include "classbuilder.h"
#include <QJsonArray>

ClassBuilder::ClassBuilder(RestBuilderXmlReader::RestClass restClass) :
	classData{std::move(restClass)},
	data{classData},
	elements{classData},
	isApi{false}
{}

ClassBuilder::ClassBuilder(RestBuilderXmlReader::RestApi restApi) :
	apiData{std::move(restApi)},
	data{apiData},
	elements{apiData},
	isApi{true}
{}

void ClassBuilder::build()
{
	// data preprocessing
	data.includes = QList<RestBuilderXmlReader::Include>{
		{false, QStringLiteral("QtCore/QString")},
		{false, QStringLiteral("QtCore/QStringList")},
		{false, QStringLiteral("QtCore/QScopedPointer")},
		{false, QStringLiteral("QtRestClient/RestClient")},
		{false, QStringLiteral("QtRestClient/RestClass")}
	} + data.includes;
	if (data.qmlUri && isApi)
		data.includes.append({false, QStringLiteral("QtQml/QQmlParserStatus")});

	for (auto &method : elements.methods) {
		if (!method.except)
			method.except = data.except;

		if (!method.postParams)
			method.postParams = method.verb == QStringLiteral("POST") && !method.body;
		if (method.postParams.value() && method.body)
			throw GeneralException("You cannot have post params AND a method body at the same time");
	}

	if (!isApi || !apiData.globalName)
		apiData.autoCreate = false;

	// cpp code generation
	if (isApi)
		generateApi();
	else
		generateClass();
}

void ClassBuilder::generateClass()
{
	//write header
	writeClassBeginDeclaration();
	writeClassMainDeclaration();
	header << "};\n\n";
	if (data.qmlUri)
		writeQmlDeclaration();
	if (data.nspace)
		header << "}\n\n";
	if (data.qmlUri)
		header << "Q_DECLARE_METATYPE(" << nsName(QStringLiteral("Qml") + data.name, data.nspace) << ")\n\n";

	//write source
	writeClassBeginDefinition();
	writeClassMainDefinition();
	if(data.qmlUri)
		writeQmlDefinitions();
	writeStartupCode();
}

void ClassBuilder::generateApi()
{
	// write header
	writeClassBeginDeclaration();
	header << "\tstatic " << data.name << "::Factory factory();\n\n";
	header << "\t" << data.name << "(QObject *parent = nullptr);\n";
	writeClassMainDeclaration();
	header << "\n\tstatic QtRestClient::RestClient *generateClient();\n"
		   << "};\n\n";
	if (data.qmlUri)
		writeQmlDeclaration();
	if (data.nspace)
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
	if (apiData.globalName)
		writeGlobalApiGeneration();
	else
		writeLocalApiGeneration();
	if (data.qmlUri)
		writeQmlDefinitions();
	writeStartupCode();
}

void ClassBuilder::writeClassBeginDeclaration()
{
	writeIncludes(data.includes);
	if(data.nspace)
		header << "namespace " << data.nspace.value() << " {\n\n";
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

	for(const auto &subClass : qAsConst(elements.classes))
		header << "\t" << subClass.type << " *" << subClass.key << "() const;\n";
	if(!elements.classes.isEmpty())
		header << '\n';

	for(const auto &method : qAsConst(elements.methods)) {
		header << "\tQtRestClient::GenericRestReply<" << method.returns << ", " << method.except.value() << "> *" << method.name
			   << "(" << writeMethodParams(method, true) << ");\n";
	}
	if(!elements.methods.isEmpty())
		header << '\n';

	header << "\tvoid setErrorTranslator(std::function<QString(" << data.except << ", int)> fn);\n\n";
	header << "Q_SIGNALS:\n"
		   << "\tvoid apiError(const QString &errorString, int errorCode, QtRestClient::RestReply::Error errorType);\n\n"
		   << "private:\n"
		   << "\tQScopedPointer<" << data.name << "Private> d;\n";
}

void ClassBuilder::writeClassBeginDefinition()
{
	source << "#include \"" << fileName << ".h\"\n\n";
	if (isApi || data.qmlUri) {
		source << "#include <QtCore/QCoreApplication>\n"
			   << "#include <QtCore/QTimer>\n";
	}
	if (data.qmlUri) {
		source << "#include <QtQml/QQmlEngine>\n"
			   << "#include <QtQml/QQmlContext>\n";
	}
	if (isApi && apiData.authenticator)
		source << "#include <QtRestClientAuth/AuthRestClient>\n";
	source << "#include <QtCore/QPointer>\n"
		   << "using namespace QtRestClient;\n";
	if (data.nspace)
		source << "using namespace " << data.nspace.value() << ";\n";
	if (isApi || !classData.path)
		source << "\nconst QString " << data.name << "::Path;\n";
	else
		source << "\nconst QString " << data.name << "::Path{" << writeExpression(classData.path.value(), true) << "};\n";
	writePrivateDefinitions();
	writeFactoryDefinition();
}

void ClassBuilder::writeClassMainDefinition()
{
	source << "\n" << data.name << "::" << data.name << "(RestClass *restClass, QObject *parent) :\n"
		   << "\t" << data.base << "{parent},\n"
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

QString ClassBuilder::writeMethodParams(const RestBuilderXmlReader::Method &method, bool asHeader)
{
	QStringList parameters;
	if(method.body)
		parameters.append(QStringLiteral("const ") + method.body.value() + QStringLiteral(" &__body"));
	// add path parameters
	if(method.path &&
	   nonstd::holds_alternative<RestBuilderXmlReader::PathGroup>(method.path.value())) {
		for(const auto &seg : qAsConst(nonstd::get<RestBuilderXmlReader::PathGroup>(method.path.value()).segments)) {
			if(nonstd::holds_alternative<RestBuilderXmlReader::BaseParam>(seg)) {
				const auto &param = nonstd::get<RestBuilderXmlReader::BaseParam>(seg);
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
		   << "\t\tFactory(Factory &&other) noexcept;\n"
		   << "\t\tFactory &operator=(Factory &&other) noexcept;\n"
		   << "\t\t~Factory();\n\n";
	for(const auto &subClass : qAsConst(elements.classes))
		header << "\t\t" << subClass.type << "::Factory " << subClass.key << "() const;\n";
	if(!elements.classes.isEmpty())
		header << '\n';
	header << "\t\t" << data.name << " *instance(QObject *parent = nullptr) const;\n\n"
		   << "\tprivate:\n"
		   << "\t\tQScopedPointer<" << data.name << "PrivateFactory> d;\n"
		   << "\t};\n\n";
}

void ClassBuilder::writePrivateDefinitions()
{
	// class
	if (data.nspace)
		source << "\nnamespace " << data.nspace.value() << " {\n";
	source << "\nclass " << data.name << "Private\n"
		   << "{\n"
		   << "public:\n"
		   << "\tinline " << data.name << "Private(RestClass *restClass) : \n"
		   << "\t\trestClass{restClass}\n"
		   << "\t{}\n\n"
		   << "\tRestClass *restClass;\n"
		   << "\tstd::function<QString(" << data.except << ", int)> errorTranslator;\n";
	for (const auto &subClass : qAsConst(elements.classes))
		source << '\t' << subClass.type << " *" << subClass.key << " = nullptr;\n";
	source << "};\n\n";

	// factory
	source << "class " << data.name << "PrivateFactory\n"
		   << "{\n"
		   << "public:\n"
		   << "\tinline " << data.name << "PrivateFactory(RestClient *client, QStringList &&subPath) :\n"
		   << "\t\tclient{client},\n"
		   << "\t\tsubPath{std::move(subPath)}\n"
		   << "\t{}\n\n"
		   << "\tRestClient *client;\n"
		   << "\tQStringList subPath;\n"
		   << "};\n\n";

	if (data.nspace)
		source << "}\n";
}

void ClassBuilder::writeFactoryDefinition()
{
	source << "\n" << data.name << "::Factory::Factory(RestClient *client, QStringList &&parentPath) :\n"
		   << "\td{new " << data.name << "PrivateFactory{client, std::move(parentPath)}}\n"
		   << "{\n"
		   << "\td->subPath.append(" << data.name << "::Path);\n"
		   << "}\n\n"
		   << data.name << "::Factory::Factory(Factory &&other) noexcept\n"
		   << "{\n"
		   << "\td.swap(other.d);\n"
		   << "}\n\n"
		   << data.name << "::Factory &" << data.name << "::Factory::operator=(Factory &&other) noexcept\n"
		   << "{\n"
		   << "\td.swap(other.d);\n"
		   << "\treturn *this;\n"
		   << "}\n\n"
		   << data.name << "::Factory::~Factory() = default;\n";
	for (const auto &subClass : qAsConst(elements.classes)) {
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
	for(const auto &subClass : qAsConst(elements.classes)) {
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
	for(const auto &method : qAsConst(elements.methods)) {
		source << "\nQtRestClient::GenericRestReply<" << method.returns << ", " << method.except.value() << "> *" << data.name << "::" << method.name
			   << "(" << writeMethodParams(method, false) << ")\n"
			   << "{\n";

		//create parameters
		if(method.path)
			writeMethodPath(method.path.value());
		source << "\tQVariantHash __params {\n";
		for(const auto &param : method.params)
			source << "\t\t{QStringLiteral(\"" << param.key << "\"), QVariant::fromValue(" << param.key << ")},\n";
		source << "\t};\n\n";
		source << "\tHeaderHash __headers {\n";
		for(const auto &header : method.headers)
			source << "\t\t{\"" << header.key << "\", " << writeExpression(header, false) << "},\n";
		source << "\t};\n\n";

		//make call
		source << "\tauto __reply = d->restClass->call<" << method.returns << ", " << method.except.value() << ">(\"" << method.verb << "\", ";
		if(method.path)
			source << "__path, ";
		if(method.body)
			source << "__body, ";
		source << "__params, __headers";
		if(method.postParams.value())
			source << ", true";
		source << ");\n";

		if(method.except == data.except) {
			source << "\tQPointer<" << data.name << "> __this(this);\n"
				   << "\t__reply->onAllErrors([__this](const QString &__e, int __c, RestReply::Error __t){\n"
				   << "\t\tif(__this)\n"
				   << "\t\t\temit __this->apiError(__e, __c, __t);\n"
				   << "\t}, [__this](" << method.except.value() << " __o, int __c){\n"
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
	if(!apiData.autoCreate && !data.qmlUri)
		return;

	source << "\nnamespace {\n\n"
		   << "void __" << data.name << "_app_construct()\n"
		   << "{\n";

	if(apiData.autoCreate)
		source << "\tQTimer::singleShot(0, &" << data.name << "::factory);\n";
	if(data.qmlUri) {
		auto uriParts = data.qmlUri.value().split(QLatin1Char(' '), QString::SkipEmptyParts);
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
		   << "\tauto client = apiClient(QStringLiteral(\"" << apiData.globalName.value() << "\"));\n"
		   << "\tif(!client) {\n"
		   << "\t\t";
	writeApiCreation();
	source << "\t\taddGlobalApi(QStringLiteral(\"" << apiData.globalName.value() << "\"), client);\n"
		   << "\t}\n"
		   << "\treturn client;\n"
		   << "}\n";
}

void ClassBuilder::writeApiCreation()
{
	QByteArray dataMode;
	switch (apiData.dataMode) {
		case RestBuilderXmlReader::Json:
			dataMode = "RestClient::DataMode::Json";
			break;
		case RestBuilderXmlReader::Cbor:
			dataMode = "RestClient::DataMode::Cbor";
			break;
	}

	if (apiData.authenticator)
		source << "client = new AuthRestClient(" << dataMode << ", " << *apiData.authenticator << ", QCoreApplication::instance());\n";
	else
		source << "client = new RestClient(" << dataMode << ", QCoreApplication::instance());\n";
	source << "\t\tclient->setAsync(" << (apiData.async ? "true" : "false") << ");\n"
		   << "\t\tclient->setBaseUrl(QUrl{" << writeExpression(apiData.baseUrl, true) << "});\n";
	if (apiData.baseUrl.apiVersion) {
		auto vNum = QVersionNumber::fromString(apiData.baseUrl.apiVersion.value());
		QStringList args;
		args.reserve(vNum.segmentCount());
		for(const auto &segment : vNum.segments()) // clazy:exclude=range-loop
			args.append(QString::number(segment));
		source << "\t\tclient->setApiVersion(QVersionNumber{" << args.join(QStringLiteral(", ")) << "});\n";
	}
	for (const auto &header : qAsConst(apiData.headers))
		source << "\t\tclient->addGlobalHeader(\"" << header.key << "\", " << writeExpression(header, false) << ");\n";
	for (const auto &param : qAsConst(apiData.params))
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
	for(const auto &subClass : qAsConst(elements.classes))
		header << "\tQ_PROPERTY(" << nsInject(subClass.type, QStringLiteral("Qml")) << " " << subClass.key << " READ " << subClass.key << " CONSTANT)\n";
	if(!elements.classes.isEmpty())
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

	for(const auto &subClass : qAsConst(elements.classes))
		header << "\t" << nsInject(subClass.type, QStringLiteral("Qml")) << " " << subClass.key << "() const;\n";
	if(!elements.classes.isEmpty())
		header << '\n';

	for(const auto &method : qAsConst(elements.methods)) {
		header << "\tQ_INVOKABLE QtRestClient::QmlGenericRestReply *" << method.name
			   << "(" << writeMethodParams(method, true) << ");\n";
	}
	if(!elements.methods.isEmpty())
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

	for(const auto &subClass : qAsConst(elements.classes)) {
		source << "\n" << nsInject(subClass.type, QStringLiteral("Qml")) << " Qml" << data.name << "::" << subClass.key << "() const\n"
			   << "{\n"
			   << "\treturn {_d ? _d->"  << subClass.key << "() : nullptr, _engine};\n"
			   << "}\n";
	}

	for(const auto &method : qAsConst(elements.methods)) {
		source << "\nQtRestClient::QmlGenericRestReply *Qml" << data.name << "::" << method.name
			   << "(" << writeMethodParams(method, false) << ")\n"
			   << "{\n"
			   << "\tif(!_d)\n"
			   << "\t\treturn nullptr;\n"
			   << "\tauto reply = _d->" << method.name << "(";

		QStringList parameters;
		if(method.body)
			parameters.append(QStringLiteral("__body"));
		// add path parameters
		if(method.path &&
		   nonstd::holds_alternative<RestBuilderXmlReader::PathGroup>(method.path.value())) {
			for(const auto &seg : qAsConst(nonstd::get<RestBuilderXmlReader::PathGroup>(method.path.value()).segments)) {
				if(nonstd::holds_alternative<RestBuilderXmlReader::BaseParam>(seg))
					parameters.append(nonstd::get<RestBuilderXmlReader::BaseParam>(seg).key);
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
			   << "\t\t->newInstance(Q_ARG(QtJsonSerializer::SerializerBase*, _d->restClient()->serializer()),\n"
			   << "\t\t\tQ_ARG(QJSEngine*, reinterpret_cast<QJSEngine*>(_engine)),\n"
			   << "\t\t\tQ_ARG(int, QMetaType::type(\"" << method.returns << "\")),\n"
			   << "\t\t\tQ_ARG(int, QMetaType::type(\"" << method.except.value() << "\")),\n"
			   << "\t\t\tQ_ARG(QtRestClient::RestReply*, reply)));\n"
			   << "}\n";
	}
}

void ClassBuilder::writeMethodPath(const RestBuilderXmlReader::variant<RestBuilderXmlReader::PathGroup, RestBuilderXmlReader::Expression> &info)
{
	if(nonstd::holds_alternative<RestBuilderXmlReader::PathGroup>(info)) {
		auto pathList = nonstd::get<RestBuilderXmlReader::PathGroup>(info);
		if(pathList.segments.isEmpty() && pathList.path)
			pathList.segments.append(RestBuilderXmlReader::Expression{pathList.expr, pathList.path.value()});

		source << "\tQString __path = QStringList {\n";
		for(const auto &seg : qAsConst(pathList.segments)) {
			if(nonstd::holds_alternative<RestBuilderXmlReader::BaseParam>(seg))
				source << "\t\tQVariant::fromValue(" << nonstd::get<RestBuilderXmlReader::BaseParam>(seg).key << ").toString(),\n";
			else if(nonstd::holds_alternative<RestBuilderXmlReader::Expression>(seg))
				source << "\t\t" << writeExpression(nonstd::get<RestBuilderXmlReader::Expression>(seg), true) << ",\n";
		}
		source << "\t}.join(QLatin1Char('/'));\n\n";
	} else if(nonstd::holds_alternative<RestBuilderXmlReader::Expression>(info))
		source << "\tQUrl __path {" << writeExpression(nonstd::get<RestBuilderXmlReader::Expression>(info), true) << "};\n\n";
}
