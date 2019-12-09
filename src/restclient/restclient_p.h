#ifndef QTRESTCLIENT_QRESTCLIENT_P_H
#define QTRESTCLIENT_QRESTCLIENT_P_H

#include "restclient.h"
#include "standardpaging_p.h"

#include <optional>

#include <QtCore/QReadWriteLock>

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
#include <QtJsonSerializer/SerializerBase>
#endif

#include <QtCore/private/qobject_p.h>

namespace QtRestClient {

class Q_RESTCLIENT_EXPORT RestClientPrivate : public QObjectPrivate
{
	Q_DECLARE_PUBLIC(RestClient)
public:
	using DataMode = RestClient::DataMode;

	static QHash<QString, RestClient*> globalApis;

	QUrl baseUrl;
	QVersionNumber apiVersion;
	HeaderHash headers;
	QUrlQuery query;
	QHash<QNetworkRequest::Attribute, QVariant> attribs;
	QAtomicPointer<QReadWriteLock> threadLock {nullptr};
#ifndef QT_NO_SSL
	QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
#endif
#ifdef QT_RESTCLIENT_USE_ASYNC
	QPointer<QThreadPool> asyncPool;
#endif

	QNetworkAccessManager *nam = nullptr;
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	QtJsonSerializer::SerializerBase *serializer = nullptr;
#else
	DataMode dataMode = DataMode::Json;
#endif

	QScopedPointer<IPagingFactory> pagingFactory {};

	RestClass *rootClass = nullptr;

	~RestClientPrivate() override;
};

}

#endif // QTRESTCLIENT_QRESTCLIENT_P_H
