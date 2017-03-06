#ifndef CLASSBUILDER_H
#define CLASSBUILDER_H

#include "restbuilder.h"

class ClassBuilder : public RestBuilder
{
public:
	ClassBuilder();

private:
	struct MethodInfo {
		QString path;
		QString verb;
		QList<QPair<QString, QString>> pathParams;
		QList<QPair<QString, QString>> parameters;
		QHash<QString, QString> headers;
		QString body;
		QString returns;
		QString except;

		MethodInfo();
	};

	QHash<QString, QString> classes;
	QHash<QString, MethodInfo> methods;

	void build() override;
	QString specialPrefix() override;

	void generateClass();
	void generateApi();

	void readClasses();
	void readMethods();

	void generateFactory();
	void writeFactoryDeclarations();
	void writeClassDeclarations();
	void writeMemberDeclarations();
};

#endif // CLASSBUILDER_H
