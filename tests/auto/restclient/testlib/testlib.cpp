#include "testlib.h"

QtRestClient::RestClient *Testlib::createClient(QObject *parent)
{
	auto client = new QtRestClient::RestClient(parent);
	client->setModernAttributes();
	client->addRequestAttribute(QNetworkRequest::HTTP2AllowedAttribute, false);
	return client;
}
