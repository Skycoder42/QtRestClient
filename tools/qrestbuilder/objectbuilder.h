#ifndef OBJECTBUILDER_H
#define OBJECTBUILDER_H

#include "restbuilder.h"

class ObjectBuilder : public RestBuilder
{
public:
	ObjectBuilder(RestBuilderXmlReader::RestObject restObject);
	ObjectBuilder(RestBuilderXmlReader::RestGadget restGadget);

private:
	bool isObject;
	RestBuilderXmlReader::RestContent data;

	void build() override;

	void generateApiObject();
	void generateApiGadget();

	QString setter(const QString &name);

	void writeEnums();
	void writeFlagOperators();
	void writeProperties();
	void writeReadDeclarations();
	void writeWriteDeclarations();
	void writeResetDeclarations();
	void writeMemberDeclarations();
	void writeEqualsDeclaration();
	void writeSourceIncludes();
	void writeReadDefinitions();
	void writeWriteDefinitions();
	void writeResetDefinitions();
	void writeEqualsDefinition();
	void writePrivateClass();
	void writeDataClass();
	void writeMemberDefinitions(bool skipComma);
	void writeSetupHooks();
};

#endif // OBJECTBUILDER_H
