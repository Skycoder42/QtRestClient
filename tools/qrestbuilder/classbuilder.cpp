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
	generateFactory();
	header << "\t" << className << "(QtRestClient::RestClass *restClass, QObject *parent);\n";
	writeClassDeclarations();
	writeMemberDeclarations();
	header << "};\n\n";
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

void ClassBuilder::generateFactory()
{
	header << "\tstatic const QString Path;\n\n"
		   << "\tclass Factory\n"
		   << "\t{\n"
		   << "\tpublic:\n"
		   << "\t\tFactory(QtRestClient::RestClient *client, const QStringList &subPath);\n\n";
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
		header << "\n\t" << it.value() << " *" << it.key() << "() const;";
	if(!classes.isEmpty())
		header << '\n';
}

void ClassBuilder::writeMemberDeclarations()
{
	for(auto it = methods.constBegin(); it != methods.constEnd(); it++) {
		header << "\n\tQtRestClient::GenericRestReply<" << it->returns << "," << it->except << "> *" << it.key() << "(";
		QStringList parameters;
		foreach(auto path, it->pathParams)
			parameters.append(path.second + " " + path.first);
		foreach(auto param, it->parameters)
			parameters.append(param.second + " " + param.first);
		header << parameters.join(", ") << ") const;";
	}
	if(!methods.isEmpty())
		header << '\n';
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
