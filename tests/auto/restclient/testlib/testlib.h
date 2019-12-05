#ifndef TESTLIB_H
#define TESTLIB_H

#include <QtRestClient>
#include <QtTest>
#include "jphpost.h"
#include "httpserver.h"

#include <variant>

class BodyType : public std::variant<QCborValue, QJsonValue>
{
public:
	BodyType() = default;
	BodyType(const BodyType &other) = default;
	BodyType(BodyType &&other) noexcept = default;
	BodyType &operator=(const BodyType &other) = default;
	BodyType &operator=(BodyType &&other) noexcept = default;

	BodyType(QCborValue data);
	BodyType(QCborMap data);
	BodyType(QCborArray data);
	BodyType(QJsonValue data);
	BodyType(QJsonObject data);
	BodyType(QJsonArray data);

	explicit BodyType(const QtRestClient::RestReply::DataType &data);

	bool isValid() const;
	QByteArray accept() const;
	void setAccept(QNetworkRequest &request) const;

	template <typename TFn>
	inline auto visit(TFn &&fn) {
		return std::visit(std::forward<TFn>(fn), static_cast<std::variant<QCborValue, QJsonValue>>(*this));
	}
	template <typename TFn>
	inline auto visit(TFn &&fn) const {
		return std::visit(std::forward<TFn>(fn), static_cast<std::variant<QCborValue, QJsonValue>>(*this));
	}

	bool operator==(const BodyType &other) const;
	bool operator!=(const BodyType &other) const;
};

QDebug operator<<(QDebug debug, const BodyType &data);

class Testlib
{
public:
	inline static BodyType CBody(QCborValue value = QCborValue::Undefined) {
		return BodyType{std::move(value)};
	}
	inline static BodyType CBody(const QJsonValue &value) {
		return BodyType{QCborValue::fromJsonValue(value)};
	}
	inline static BodyType JBody(QJsonValue value = QJsonValue::Undefined) {
		return BodyType{std::move(value)};
	}
	inline static BodyType JBody(const QCborValue &value) {
		return BodyType{value.toJsonValue()};
	}

	static QtRestClient::RestClient *createClient(QObject *parent = nullptr);
};

Q_DECLARE_METATYPE(QUrlQuery)
Q_DECLARE_METATYPE(QNetworkRequest::Attribute)
Q_DECLARE_METATYPE(BodyType)

#endif // TESTLIB_H
