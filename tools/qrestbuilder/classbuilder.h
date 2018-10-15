#ifndef CLASSBUILDER_H
#define CLASSBUILDER_H

#include "restbuilder.h"
#include <QVersionNumber>

class ClassBuilder : public RestBuilder
{
public:
	ClassBuilder(RestBuilderXmlReader::RestClass restClass);
	ClassBuilder(RestBuilderXmlReader::RestApi restApi);

private:
	RestBuilderXmlReader::RestClass classData;
	RestBuilderXmlReader::RestApi apiData;
	RestBuilderXmlReader::RestAccess &data;
	bool isApi;

	void build() override;

	void generateClass();
	void generateApi();

	void writeClassBeginDeclaration();
	void writeClassMainDeclaration();
	void writeClassBeginDefinition();
	void writeClassMainDefinition();

	QString writeExpression(const RestBuilderXmlReader::Expression &expression, bool asString);
	QString writeMethodParams(const RestBuilderXmlReader::Method &method, bool withDefaults);

	void writeFactoryDeclaration();
	void writePrivateDefinitions();
	void writeFactoryDefinition();
	void writeClassDefinitions();
	void writeMethodDefinitions();
	void writeMemberDefinitions();
	void writeStartupCode();

	void writeLocalApiGeneration();
	void writeGlobalApiGeneration();
	void writeApiCreation();

	void writeQmlDeclaration();
	void writeQmlDefinitions();

	void writeMethodPath(const RestBuilderXmlReader::variant<RestBuilderXmlReader::PathGroup, RestBuilderXmlReader::Expression> &info);
};

#endif // CLASSBUILDER_H
