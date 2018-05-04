#ifndef OBJECTBUILDER_H
#define OBJECTBUILDER_H

#include "restbuilder.h"

class ObjectBuilder : public RestBuilder
{
	Q_OBJECT

public:
	explicit ObjectBuilder(QObject *parent = nullptr);

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
	void writeEqualsDeclaration(bool asGadget);
	void writeNotifyDeclarations();
	void writeMemberDeclarations(QTextStream &stream);
	void writeSourceIncludes();
	void writeReadDefinitions(bool asGadget);
	void writeWriteDefinitions(bool asGadget);
	void writeEqualsDefinition(bool asGadget);
	void writeDataClass();
	void writeMemberDefinitions(QTextStream &stream);
	void writeMemberCopyDefinitions(QTextStream &stream);
	void writeListConverter(bool asGadget);
};

#endif // OBJECTBUILDER_H
