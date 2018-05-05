#ifndef OBJECTBUILDER_H
#define OBJECTBUILDER_H

#include "restbuilder.h"

class ObjectBuilder : public RestBuilder
{
	Q_OBJECT

public:
	explicit ObjectBuilder(QXmlStreamReader &inStream, QObject *parent = nullptr);

	static bool canReadType(const QString &type);

private:
	QHash<QString, QString> members;
	bool testEquality;

	void build() override;

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
	void writeMemberDeclarations(QTextStream &reader);
	void writeSourceIncludes();
	void writeReadDefinitions(bool asGadget);
	void writeWriteDefinitions(bool asGadget);
	void writeEqualsDefinition(bool asGadget);
	void writeDataClass();
	void writeMemberDefinitions(QTextStream &reader);
	void writeMemberCopyDefinitions(QTextStream &reader);
	void writeListConverter(bool asGadget);
};

#endif // OBJECTBUILDER_H
