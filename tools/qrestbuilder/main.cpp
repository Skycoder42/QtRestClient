#include "classbuilder.h"
#include "objectbuilder.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QLibraryInfo>
#include <QTranslator>
#include <QXmlStreamReader>

#include "xmlconverter.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QCoreApplication::setApplicationName(QStringLiteral(TARGET));
	QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
	QCoreApplication::setOrganizationName(QStringLiteral(COMPANY));
	QCoreApplication::setOrganizationDomain(QStringLiteral(BUNDLE_PREFIX));

	QStringList trList = {QStringLiteral("qtbase"), QStringLiteral("qrestbuilder")};
	for(const auto &trFile : trList) {
		auto translator = new QTranslator(&a);
		if(translator->load(QLocale(),
							trFile,
							QStringLiteral("_"),
							QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
			a.installTranslator(translator);
		else {
			qWarning() << "Failed to load translations file" << trFile;
			delete translator;
		}
	}

	QCommandLineParser parser;
	parser.setApplicationDescription(QCoreApplication::translate("PARSER", "A tool to create code for a rest API based on an API description"));
	parser.addVersionOption();
	parser.addHelpOption();

	parser.addOption({
						 QStringLiteral("class"),
						 QCoreApplication::translate("PARSER", "Set the builders mode to build an api class")
					 });
	parser.addOption({
						 QStringLiteral("object"),
						 QCoreApplication::translate("PARSER", "Set the builders mode to build an api object")
					 });
	parser.addOption({
						 {QStringLiteral("c"), QStringLiteral("convert")},
						 QCoreApplication::translate("PARSER", "Convert a legacy json file of <type> to the new XML format. "
						 "Use --impl to specify the name of the RC-XML file to be created."),
						 QCoreApplication::translate("PARSER", "type")
					 });
	parser.addOption({
						 QStringLiteral("in"),
						 QCoreApplication::translate("PARSER", "The input JSON <file> containing the API definition"),
						 QCoreApplication::translate("PARSER", "file")
					 });
	parser.addOption({
						 QStringLiteral("header"),
						 QCoreApplication::translate("PARSER", "The <name> of the header file to generate"),
						 QCoreApplication::translate("PARSER", "name")
					 });
	parser.addOption({
						 QStringLiteral("impl"),
						 QCoreApplication::translate("PARSER", "The <name> of the implementation file to generate"),
						 QCoreApplication::translate("PARSER", "name")
					 });

	parser.process(a);

	try {
		if(parser.isSet(QStringLiteral("convert"))) {
			XmlConverter converter;
			converter.convert(parser.value(QStringLiteral("convert")),
							  parser.value(QStringLiteral("in")),
							  parser.value(QStringLiteral("impl")));
			return EXIT_SUCCESS;
		}

		QScopedPointer<RestBuilder> builder;
		if(parser.isSet(QStringLiteral("class")))
			builder.reset(new ClassBuilder());
		else if(parser.isSet(QStringLiteral("object")))
			builder.reset(new ObjectBuilder());
		else
			throw QCoreApplication::translate("PARSER", "Invalid mode! You must specify either --class or --object");

		builder->build(parser.value(QStringLiteral("in")),
					   parser.value(QStringLiteral("header")),
					   parser.value(QStringLiteral("impl")));
		return EXIT_SUCCESS;
	} catch (const QString &str) {
		qCritical() << qUtf8Printable(str);
		return EXIT_FAILURE;
	}
}
