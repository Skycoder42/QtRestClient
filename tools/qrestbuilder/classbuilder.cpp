#include "classbuilder.h"
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

	//auto includes = scanIncludes();
	//includes.insert("QtRestClient::RestClass", "restclass.h");

	//write header
	//writeIncludes(header, includes.values());
	header << "class " << className << "\n"
		   << "{\n"
		   << "};\n\n";
}

void ClassBuilder::generateApi()
{
	qInfo() << "generating api:" << className;
}
