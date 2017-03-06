#ifndef OBJECTBUILDER_H
#define OBJECTBUILDER_H

#include "restbuilder.h"

class ObjectBuilder : public RestBuilder
{
public:
	explicit ObjectBuilder();

private:
	QHash<QString, QString> includes;
	QHash<QString, QString> members;

	void build() override;

	void generateApiObject(const QString &name);
	void generateApiGadget(const QString &name);
	void readMembers();
};

#endif // OBJECTBUILDER_H
