#ifndef OBJECTBUILDER_H
#define OBJECTBUILDER_H

#include "restbuilder.h"

class ObjectBuilder : public RestBuilder
{
public:
	ObjectBuilder(RestBuilderXmlReader::RestObject restObject);
	ObjectBuilder(RestBuilderXmlReader::RestGadget restGadget);

private:
	struct SudoProperty {
		const RestBuilderXmlReader::TypedVariableAttribs &basic;
		const RestBuilderXmlReader::PropertyAttribs &attribs;
	};

	bool isObject;
	RestBuilderXmlReader::RestContent data;

	void build() override;

	void generateApiObject();
	void generateApiGadget();

	QString setter(const QString &name);
	const RestBuilderXmlReader::TypedVariableAttribs &propertyBasics(const nonstd::variant<RestBuilderXmlReader::Property, RestBuilderXmlReader::UserProperty> &prop) const;
	const RestBuilderXmlReader::PropertyAttribs &propertyAttribs(const nonstd::variant<RestBuilderXmlReader::Property, RestBuilderXmlReader::UserProperty> &prop) const;
	SudoProperty sudoProperty(const nonstd::variant<RestBuilderXmlReader::Property, RestBuilderXmlReader::UserProperty> &prop) const;

	void writeEnums();
	void writeFlagOperators();
	void writeProperties();
	void writeAggregateConstructorDeclaration();
	void writeReadDeclarations();
	void writeWriteDeclarations();
	void writeResetDeclarations();
	void writeEqualsDeclaration();
	void writeQHashDeclaration(bool asFriend);
	void writeSourceIncludes();
	void writeAggregateConstructorDefinition();
	void writeReadDefinitions();
	void writeWriteDefinitions();
	void writeResetDefinitions();
	void writeEqualsDefinition();
	void writeQHashDefinition();
	void writePrivateClass();
	void writeDataClass();
	void writeMemberDefinitions();
	void writeSetupHooks();
};

#endif // OBJECTBUILDER_H
