#ifndef OBJECTBUILDER_H
#define OBJECTBUILDER_H

#include "restbuilder.h"

class ObjectBuilder : public RestBuilder
{
public:
	explicit ObjectBuilder();

private:
	TypeInfo parentType;
	QHash<QString, QString> includes;
	QHash<QString, TypeInfo> members;

	void build() override;

	void generateApiObject(const QString &name);
	void generateApiGadget(const QString &name);
	void readMembers();

	void writeProperties(bool withNotify);
};

#endif // OBJECTBUILDER_H
