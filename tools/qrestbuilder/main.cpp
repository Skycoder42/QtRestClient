#include "restbuilder.h"

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
	QCoreApplication::setOrganizationDomain(DOMAIN);

	QCommandLineParser parser;
	parser.setApplicationDescription("A tool to create code for a rest API based on an API description");
	parser.addVersionOption();
	parser.addHelpOption();

	parser.addOption({
						 "in",
						 "The input JSON file containing the API definition"
					 });
	parser.addOption({
						 "header",
						 "The name of the header file to generate"
					 });
	parser.addOption({
						 "impl",
						 "The name of the implementation file to generate"
					 });

	parser.process(a);

	RestBuilder builder;
	return builder.build(parser.value("in"), parser.value("header"), parser.value("impl"));
}
