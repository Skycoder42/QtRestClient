#ifndef QTRESTCLIENT_STANDARDPAGING_P_H
#define QTRESTCLIENT_STANDARDPAGING_P_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/ipaging.h"

#include <optional>
#include <limits>

#include <QtCore/QSharedDataPointer>

namespace QtRestClient {

template <typename TPagingBase>
class StandardPagingBase : public TPagingBase
{
	static_assert (std::is_base_of_v<IPaging, TPagingBase>, "TPagingBase must inherit or be the IPaging interface");
	friend class StandardPagingFactory;
public:
	qint64 total() const override;
	qint64 offset() const override;
	bool hasNext() const override;
	QUrl next() const override;
	bool hasPrevious() const override;
	QUrl previous() const override;

protected:
	qint64 _total = std::numeric_limits<qint64>::max();
	qint64 _offset = -1;
	QUrl _prev;
	QUrl _next;
};

class Q_RESTCLIENT_EXPORT StandardCborPaging : public StandardPagingBase<ICborPaging>
{
	friend class StandardPagingFactory;
public:
	QVariantMap properties() const override;
	QCborArray cborItems() const override;
	QCborValue originalCbor() const override;

private:
	QCborArray _items;
	QCborValue _data;
};

class Q_RESTCLIENT_EXPORT StandardJsonPaging : public StandardPagingBase<IJsonPaging>
{
	friend class StandardPagingFactory;
public:
	QVariantMap properties() const override;
	QJsonArray jsonItems() const override;
	QJsonValue originalJson() const override;

private:
	QJsonArray _items;
	QJsonValue _data;
};

class Q_RESTCLIENT_EXPORT StandardPagingFactory : public IPagingFactory
{
public:
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	IPaging *createPaging(QtJsonSerializer::SerializerBase *serializer, const std::variant<QCborValue, QJsonValue> &data) const override;
#else
	IPaging *createPaging(const std::variant<QCborValue, QJsonValue> &data) const override;
#endif

private:
	static std::optional<QUrl> extractUrl(const std::variant<QCborValue, QJsonValue> &value);
};

// ------------- generic implementation -------------

template <typename TPagingBase>
qint64 StandardPagingBase<TPagingBase>::total() const
{
	return _total;
}

template <typename TPagingBase>
qint64 StandardPagingBase<TPagingBase>::offset() const
{
	return _offset;
}

template <typename TPagingBase>
bool StandardPagingBase<TPagingBase>::hasNext() const
{
	return _next.isValid();
}

template <typename TPagingBase>
QUrl StandardPagingBase<TPagingBase>::next() const
{
	return _next;
}

template <typename TPagingBase>
bool StandardPagingBase<TPagingBase>::hasPrevious() const
{
	return _prev.isValid();
}

template <typename TPagingBase>
QUrl StandardPagingBase<TPagingBase>::previous() const
{
	return _prev;
}

}

#endif // QTRESTCLIENT_STANDARDPAGING_P_H
