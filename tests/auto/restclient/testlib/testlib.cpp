#include "testlib.h"
using namespace QtRestClient;

QtRestClient::RestClient *Testlib::createClient(QObject *parent)
{
	auto client = new QtRestClient::RestClient(parent);
	client->setModernAttributes();
	client->addRequestAttribute(QNetworkRequest::HTTP2AllowedAttribute, false);
	return client;
}

void Testlib::setAccept(QNetworkRequest &request, RestClient *client)
{
	switch (client->dataMode()) {
	case RestClient::DataMode::Cbor:
		request.setRawHeader("Accept", "application/cbor");
		break;
	case RestClient::DataMode::Json:
		request.setRawHeader("Accept", "application/json");
		break;
	}
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

BodyType BodyType::parse(QNetworkReply *reply)
{
	const auto contentType = reply->header(QNetworkRequest::ContentTypeHeader).toByteArray();
	if (contentType == "application/cbor") {
		QCborStreamReader reader{reply};
		auto repData = QCborValue::fromCbor(reader);
		if (reader.lastError() != QCborError::NoError)
			return Testlib::CBody();
		else
			return repData;
	} else if (contentType == "application/json") {
		QJsonParseError error;
		auto repData = QJsonDocument::fromJson(reply->readAll(), &error);
		if (error.error != QJsonParseError::NoError)
			return Testlib::JBody();
		else if (repData.isObject())
			return repData.object();
		else if (repData.isArray())
			return repData.array();
		else
			return QJsonValue{QJsonValue::Null};
	} else
		return {};
}

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

QVariant BodyType::toVariant() const
{
	return visit([](const auto &data){
		return data.toVariant();
	});
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
