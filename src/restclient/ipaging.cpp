#include "ipaging.h"
#include <limits>
using namespace QtRestClient;

IPaging::IPaging() = default;

IPaging::~IPaging() = default;

qint64 IPaging::total() const
{
	return std::numeric_limits<qint64>::max();
}

qint64 IPaging::offset() const
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

IPagingFactory::IPagingFactory() = default;

IPagingFactory::~IPagingFactory() = default;

std::variant<QCborArray, QJsonArray> ICborPaging::items() const
{
	return cborItems();
}

std::variant<QCborValue, QJsonValue> ICborPaging::originalData() const
{
	return originalCbor();
}

std::variant<QCborArray, QJsonArray> IJsonPaging::items() const
{
	return jsonItems();
}

std::variant<QCborValue, QJsonValue> IJsonPaging::originalData() const
{
	return originalJson();
}
