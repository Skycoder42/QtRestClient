#ifndef OBJECTBUILDER_H
#define OBJECTBUILDER_H

#include "restbuilder.h"

class ObjectBuilder : public RestBuilder
{
public:
	explicit ObjectBuilder();

private:
	QHash<QString, QString> members;

	void build() override;
	QString specialPrefix() override;

	void generateApiObject();
	void generateApiGadget();
	void readMembers();

	QString setter(const QString &name);

	void writeProperties(bool withNotify);
	void writeReadDeclarations();
	void writeWriteDeclarations();
	void writeNotifyDeclarations();
	void writeMemberDeclarations(QTextStream &stream);
	void writeReadDefinitions(bool asGadget);
	void writeWriteDefinitions(bool asGadget);
	void writeDataClass();
	void writeMemberDefinitions(QTextStream &stream);
	void writeMemberCopyDefinitions(QTextStream &stream);
};

#endif // OBJECTBUILDER_H
