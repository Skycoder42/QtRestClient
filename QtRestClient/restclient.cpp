#include "restclient.h"
using namespace QtRestClient;

RestClient::RestClient(QObject *parent) : QObject(parent)
{

}

QUrl RestClient::baseUrl() const
{
	return m_baseUrl;
}

QVersionNumber RestClient::apiVersion() const
{
	return m_apiVersion;
}

void RestClient::setBaseUrl(QUrl baseUrl)
{
	if (m_baseUrl == baseUrl)
		return;

	m_baseUrl = baseUrl;
	emit baseUrlChanged(baseUrl);
}

void RestClient::setApiVersion(QVersionNumber apiVersion)
{
	if (m_apiVersion == apiVersion)
		return;

	m_apiVersion = apiVersion;
	emit apiVersionChanged(apiVersion);
}
