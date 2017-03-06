#ifndef OBJECTBUILDER_H
#define OBJECTBUILDER_H

#include "restbuilder.h"

class ObjectBuilder : public RestBuilder
{
public:
	explicit ObjectBuilder();

private:
	void build(const QFileInfo &inFile) override;

	void generateApiObject(const QString &name);
	void generateApiGadget(const QString &name);
};

#endif // OBJECTBUILDER_H
