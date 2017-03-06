#ifndef CLASSBUILDER_H
#define CLASSBUILDER_H

#include "restbuilder.h"

class ClassBuilder : public RestBuilder
{
public:
	ClassBuilder();

private:
	void build() override;
	QString specialPrefix() override;

	void generateClass();
	void generateApi();
};

#endif // CLASSBUILDER_H
