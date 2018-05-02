#include "ipaging.h"
#include <limits>
using namespace QtRestClient;

IPaging::~IPaging() = default;

int IPaging::total() const
{
	return std::numeric_limits<int>::max();
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

PagingFactory::~PagingFactory() = default;
