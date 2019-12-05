#include "testlib.h"

QtRestClient::RestClient *Testlib::createClient(QObject *parent)
{
	auto client = new QtRestClient::RestClient(parent);
	client->setModernAttributes();
	client->addRequestAttribute(QNetworkRequest::HTTP2AllowedAttribute, false);
	return client;
}

BodyType::BodyType(QCborValue data) :
	variant<QCborValue, QJsonValue>{std::move(data)}
{}

BodyType::BodyType(QCborMap data) :
	variant<QCborValue, QJsonValue>{std::move(data)}
{}

BodyType::BodyType(QCborArray data) :
	variant<QCborValue, QJsonValue>{std::move(data)}
{}

BodyType::BodyType(QJsonValue data) :
	variant<QCborValue, QJsonValue>{std::move(data)}
{}

BodyType::BodyType(QJsonObject data) :
	variant<QCborValue, QJsonValue>{std::move(data)}
{}

BodyType::BodyType(QJsonArray data) :
	variant<QCborValue, QJsonValue>{std::move(data)}
{}

BodyType::BodyType(const QtRestClient::RestReply::DataType &data) :
	variant<QCborValue, QJsonValue>{std::visit(QtRestClient::__private::overload {
												   [](std::nullopt_t) -> variant<QCborValue, QJsonValue> {
													   return QCborValue{};
												   },
												   [](const auto &vData) -> variant<QCborValue, QJsonValue> {
													   return vData;
												   }
											   }, data)}
{}

bool BodyType::isValid() const
{
	return visit([](const auto &body) {
		return !body.isUndefined();
	});
}

QByteArray BodyType::accept() const
{
	if (std::holds_alternative<QCborValue>(*this))
		return "application/cbor";
	if (std::holds_alternative<QJsonValue>(*this))
		return "application/json";
	else
		Q_UNREACHABLE();
}

void BodyType::setAccept(QNetworkRequest &request) const
{
	request.setRawHeader("Accept", accept());
}

bool BodyType::operator==(const BodyType &other) const
{
	return static_cast<std::variant<QCborValue, QJsonValue>>(*this) ==
		   static_cast<std::variant<QCborValue, QJsonValue>>(other);
}

bool BodyType::operator!=(const BodyType &other) const
{
	return static_cast<std::variant<QCborValue, QJsonValue>>(*this) !=
		   static_cast<std::variant<QCborValue, QJsonValue>>(other);
}

QDebug operator<<(QDebug debug, const BodyType &data)
{
	return data.visit([&](const auto &vData) {
		return debug << vData;
	});
}
