#include "classbuilder.h"
#include "objectbuilder.h"

#include <QCoreApplication>
#include <QDir>
#include <QCommandLineParser>
#include <iostream>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QCoreApplication::setApplicationName(TARGET);
	QCoreApplication::setApplicationVersion(VERSION);
	QCoreApplication::setOrganizationName(COMPANY);
	QCoreApplication::setOrganizationDomain(BUNDLE_PREFIX);

	QCommandLineParser parser;
	parser.setApplicationDescription("A tool to create code for a rest API based on an API description");
	parser.addVersionOption();
	parser.addHelpOption();

	parser.addOption({
						 "class",
						 "Set the builders mode to build an api class"
					 });
	parser.addOption({
						 "object",
						 "Set the builders mode to build an api object"
					 });
	parser.addOption({
						 "in",
						 "The input JSON <file> containing the API definition",
						 "file"
					 });
	parser.addOption({
						 "header",
						 "The <name> of the header file to generate",
						 "name"
					 });
	parser.addOption({
						 "impl",
						 "The <name> of the implementation file to generate",
						 "name"
					 });

	parser.process(a);

	try {
		QScopedPointer<RestBuilder> builder;
		if(parser.isSet("class"))
			builder.reset(new ClassBuilder());
		else if(parser.isSet("object"))
			builder.reset(new ObjectBuilder());
		else
			throw QStringLiteral("Invalid mode! You must specify either --class or --object");

		builder->build(parser.value("in"),
					   parser.value("header"),
					   parser.value("impl"));
		return EXIT_SUCCESS;
	} catch (QString &str) {
		std::cerr << str.toStdString() << std::endl;
		return EXIT_FAILURE;
	}
}
