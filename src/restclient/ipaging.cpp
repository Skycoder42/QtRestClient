#include "ipaging.h"
using namespace QtRestClient;

IPaging::~IPaging() {}

int IPaging::total() const
{
	return INT_MAX;
}

int IPaging::offset() const
{
	return -1;
}

bool IPaging::hasPrevious() const
{
	return false;
}

QUrl IPaging::previous() const
{
	return QUrl();
}

PagingFactory::~PagingFactory() {}
