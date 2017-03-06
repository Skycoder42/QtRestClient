#ifndef CLASSBUILDER_H
#define CLASSBUILDER_H

#include "restbuilder.h"

class ClassBuilder : public RestBuilder
{
public:
	ClassBuilder();

private:
	void build() override;
};

#endif // CLASSBUILDER_H
