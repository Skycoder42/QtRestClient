#include "classbuilder.h"
#include "objectbuilder.h"

#include <QCoreApplication>
#include <QDir>
#include <QCommandLineParser>
#include <iostream>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QCoreApplication::setApplicationName(QStringLiteral(TARGET));
	QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
	QCoreApplication::setOrganizationName(QStringLiteral(COMPANY));
	QCoreApplication::setOrganizationDomain(QStringLiteral(BUNDLE_PREFIX));

	QCommandLineParser parser;
	parser.setApplicationDescription(QCoreApplication::translate("GLOBAL", "A tool to create code for a rest API based on an API description"));
	parser.addVersionOption();
	parser.addHelpOption();

	parser.addOption({
						 QStringLiteral("class"),
						 QCoreApplication::translate("GLOBAL", "Set the builders mode to build an api class")
					 });
	parser.addOption({
						 QStringLiteral("object"),
						 QCoreApplication::translate("GLOBAL", "Set the builders mode to build an api object")
					 });
	parser.addOption({
						 QStringLiteral("in"),
						 QCoreApplication::translate("GLOBAL", "The input JSON <file> containing the API definition"),
						 QCoreApplication::translate("GLOBAL", "file")
					 });
	parser.addOption({
						 QStringLiteral("header"),
						 QCoreApplication::translate("GLOBAL", "The <name> of the header file to generate"),
						 QCoreApplication::translate("GLOBAL", "name")
					 });
	parser.addOption({
						 QStringLiteral("impl"),
						 QCoreApplication::translate("GLOBAL", "The <name> of the implementation file to generate"),
						 QCoreApplication::translate("GLOBAL", "name")
					 });

	parser.process(a);

	try {
		QScopedPointer<RestBuilder> builder;
		if(parser.isSet(QStringLiteral("class")))
			builder.reset(new ClassBuilder());
		else if(parser.isSet(QStringLiteral("object")))
			builder.reset(new ObjectBuilder());
		else
			throw QCoreApplication::translate("GLOBAL", "Invalid mode! You must specify either --class or --object");

		builder->build(parser.value(QStringLiteral("in")),
					   parser.value(QStringLiteral("header")),
					   parser.value(QStringLiteral("impl")));
		return EXIT_SUCCESS;
	} catch (const QString &str) {
		std::cerr << str.toStdString() << std::endl;
		return EXIT_FAILURE;
	}
}
