#include "restobject.h"
using namespace QtRestClient;

const int RestObject::metaId()
{
	return qMetaTypeId<RestObject*>();
}

RestObject::RestObject(QObject *parent) : QObject(parent)
{

}
