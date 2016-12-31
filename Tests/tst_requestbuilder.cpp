#include "tst_requestbuilder.h"
#include <QtRestClient>

Q_DECLARE_METATYPE(QUrlQuery)
Q_DECLARE_METATYPE(QNetworkRequest::Attribute)

RequestBuilderTest::RequestBuilderTest(QObject *parent) :
	QObject(parent)
{}

void RequestBuilderTest::testBuilding_data()
{
	QTest::addColumn<QUrl>("base");
	QTest::addColumn<QVersionNumber>("version");
	QTest::addColumn<QtRestClient::HeaderHash>("headers");
	QTest::addColumn<QUrlQuery>("params");
	QTest::addColumn<QString>("path");
	QTest::addColumn<QNetworkRequest::Attribute>("attributeKey");
	QTest::addColumn<QVariant>("attributeValue");
	QTest::addColumn<QSslConfiguration>("sslConfig");
	QTest::addColumn<QUrl>("resultUrl");

	QTest::newRow("base") << QUrl("https://api.example.com/basic/")
						  << QVersionNumber()
						  << QtRestClient::HeaderHash()
						  << QUrlQuery()
						  << QString()
						  << (QNetworkRequest::Attribute)0
						  << QVariant()
						  << QSslConfiguration::defaultConfiguration()
						  << QUrl("https://api.example.com/basic");

	QTest::newRow("version") << QUrl("https://api.example.com/basic/")
							 << QVersionNumber(4,2,0)
							 << QtRestClient::HeaderHash()
							 << QUrlQuery()
							 << QString()
							 << (QNetworkRequest::Attribute)0
							 << QVariant()
							 << QSslConfiguration::defaultConfiguration()
							 << QUrl("https://api.example.com/basic/v4.2");

	QTest::newRow("header") << QUrl("https://api.example.com/basic/")
							<< QVersionNumber()
							<< QtRestClient::HeaderHash({{"Bearer", "Secret"}})
							<< QUrlQuery()
							<< QString()
							<< (QNetworkRequest::Attribute)0
							<< QVariant()
							<< QSslConfiguration::defaultConfiguration()
							<< QUrl("https://api.example.com/basic");

	QUrlQuery query;
	query.addQueryItem("p1", "baum");
	query.addQueryItem("p2", "42");
	QTest::newRow("parameters") << QUrl("https://api.example.com/basic/")
								<< QVersionNumber()
								<< QtRestClient::HeaderHash()
								<< query
								<< QString()
								<< (QNetworkRequest::Attribute)0
								<< QVariant()
								<< QSslConfiguration::defaultConfiguration()
								<< QUrl("https://api.example.com/basic?p1=baum&p2=42");

	QTest::newRow("path") << QUrl("https://api.example.com/basic/")
						  << QVersionNumber()
						  << QtRestClient::HeaderHash()
						  << QUrlQuery()
						  << QStringLiteral("/examples/exampleStuff/")
						  << (QNetworkRequest::Attribute)0
						  << QVariant()
						  << QSslConfiguration::defaultConfiguration()
						  << QUrl("https://api.example.com/basic/examples/exampleStuff");

	QTest::newRow("attribute") << QUrl("https://api.example.com/basic/")
							   << QVersionNumber()
							   << QtRestClient::HeaderHash()
							   << QUrlQuery()
							   << QString()
							   << QNetworkRequest::CacheSaveControlAttribute
							   << QVariant(false)
							   << QSslConfiguration::defaultConfiguration()
							   << QUrl("https://api.example.com/basic");

	auto config = QSslConfiguration::defaultConfiguration();
	config.setProtocol(QSsl::TlsV1_2);
	config.setPeerVerifyMode(QSslSocket::VerifyPeer);
	QTest::newRow("sslConfig") << QUrl("https://api.example.com/basic/")
							   << QVersionNumber()
							   << QtRestClient::HeaderHash()
							   << QUrlQuery()
							   << QString()
							   << (QNetworkRequest::Attribute)0
							   << QVariant()
							   << config
							   << QUrl("https://api.example.com/basic");

	QTest::newRow("full") << QUrl("https://api.example.com/basic/")
						  << QVersionNumber(4,2,0)
						  << QtRestClient::HeaderHash({{"Bearer", "Secret"}})
						  << query
						  << QStringLiteral("/examples/exampleStuff/")
						  << QNetworkRequest::CacheSaveControlAttribute
						  << QVariant(false)
						  << config
						  << QUrl("https://api.example.com/basic/v4.2/examples/exampleStuff?p1=baum&p2=42");
}

void RequestBuilderTest::testBuilding()
{
	QFETCH(QUrl, base);
	QFETCH(QVersionNumber, version);
	QFETCH(QtRestClient::HeaderHash, headers);
	QFETCH(QUrlQuery, params);
	QFETCH(QString, path);
	QFETCH(QNetworkRequest::Attribute, attributeKey);
	QFETCH(QVariant, attributeValue);
	QFETCH(QSslConfiguration, sslConfig);
	QFETCH(QUrl, resultUrl);

	auto builder = QtRestClient::RequestBuilder(nullptr, base)
				   .setVersion(version)
				   .addHeaders(headers)
				   .addParameters(params)
				   .addPath(path)
				   .setSslConfig(sslConfig);
	if(attributeValue.isValid())
		builder.setAttribute(attributeKey, attributeValue);

	auto request = builder.build();

	QCOMPARE(request.url(), resultUrl);
	for(auto it = headers.constBegin(); it != headers.constEnd(); it++)
		QCOMPARE(request.rawHeader(it.key()), it.value());
	QCOMPARE(request.sslConfiguration(), sslConfig);
	if(attributeValue.isValid())
		QCOMPARE(request.attribute(attributeKey), attributeValue);
}
