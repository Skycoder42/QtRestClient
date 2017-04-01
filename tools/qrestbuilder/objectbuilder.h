#ifndef OBJECTBUILDER_H
#define OBJECTBUILDER_H

#include "restbuilder.h"

class ObjectBuilder : public RestBuilder
{
public:
	explicit ObjectBuilder();

private:
	QHash<QString, QString> members;
	bool testEquality;

	void build() override;
	QString specialPrefix() override;

	void generateApiObject();
	void generateApiGadget();
	void readMembers();

	QString setter(const QString &name);

	void writeEnums();
	void writeFlagOperators();
	void writeProperties(bool withNotify);
	void writeReadDeclarations();
	void writeWriteDeclarations();
	void writeNotifyDeclarations();
	void writeMemberDeclarations(QTextStream &stream);
	void writeSourceIncludes();
	void writeReadDefinitions(bool asGadget);
	void writeWriteDefinitions(bool asGadget);
	void writeDataClass();
	void writeMemberDefinitions(QTextStream &stream);
	void writeMemberCopyDefinitions(QTextStream &stream);
	void writeListConverter(bool isObject);
};

#endif // OBJECTBUILDER_H
