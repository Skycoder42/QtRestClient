#include "testlib.h"

class RestClientTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();

	void testBaseUrl_data();
	void testBaseUrl();
};

void RestClientTest::initTestCase()
{
}

void RestClientTest::testBaseUrl_data()
{
	QTest::addColumn<QUrl>("base");
	QTest::addColumn<QVersionNumber>("version");
	QTest::addColumn<QtRestClient::HeaderHash>("headers");
	QTest::addColumn<QUrlQuery>("params");
	QTest::addColumn<QHash<QNetworkRequest::Attribute, QVariant>>("attributes");
	QTest::addColumn<QSslConfiguration>("sslConfig");
	QTest::addColumn<QUrl>("resultUrl");

	QUrlQuery query;
	query.addQueryItem("p1", "baum");
	query.addQueryItem("p2", "42");
	QHash<QNetworkRequest::Attribute, QVariant> attribs;
	attribs.insert(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
	auto config = QSslConfiguration::defaultConfiguration();
	config.setProtocol(QSsl::TlsV1_2);
	config.setPeerVerifyMode(QSslSocket::VerifyPeer);
	QTest::newRow("general") << QUrl("https://api.example.com/basic/")
							 << QVersionNumber(4,2,0)
							 << QtRestClient::HeaderHash({{"Bearer", "Secret"}})
							 << query
							 << attribs
							 << config
							 << QUrl("https://api.example.com/basic/v4.2?p1=baum&p2=42");
}

void RestClientTest::testBaseUrl()
{
	QFETCH(QUrl, base);
	QFETCH(QVersionNumber, version);
	QFETCH(QtRestClient::HeaderHash, headers);
	QFETCH(QUrlQuery, params);
	auto attributes = *static_cast<QHash<QNetworkRequest::Attribute, QVariant> *>(QTest::qData("attributes", ::qMetaTypeId<typename std::remove_cv<QHash<QNetworkRequest::Attribute, QVariant> >::type>()));
	QFETCH(QSslConfiguration, sslConfig);
	QFETCH(QUrl, resultUrl);

	QtRestClient::RestClient client;
	client.setBaseUrl(base);
	client.setApiVersion(version);
	client.setGlobalHeaders(headers);
	client.setGlobalParameters(params);
	client.setRequestAttributes(attributes);
	client.setSslConfiguration(sslConfig);

	auto request = client.builder().build();

	QCOMPARE(request.url(), resultUrl);
	for(auto it = headers.constBegin(); it != headers.constEnd(); it++)
		QCOMPARE(request.rawHeader(it.key()), it.value());
	for(auto it = attributes.constBegin(); it != attributes.constEnd(); it++)
		QCOMPARE(request.attribute(it.key()), it.value());
	QCOMPARE(request.sslConfiguration(), sslConfig);
}

QTEST_MAIN(RestClientTest)

#include "tst_restclient.moc"
