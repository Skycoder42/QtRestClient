#ifndef QTRESTCLIENT_RESTCLASS_H
#define QTRESTCLIENT_RESTCLASS_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/requestbuilder.h"
#include "QtRestClient/restreply.h"
#include "QtRestClient/genericrestreply.h"
#include "QtRestClient/restclient.h"

#include <QtCore/qobject.h>

namespace QtRestClient {

class RestClassPrivate;
//! A class to perform requests to an API
class Q_RESTCLIENT_EXPORT RestClass : public QObject
{
	Q_OBJECT
	friend class RestClient;
	friend class RestClientPrivate;

public:
	//! A constant for the HTTP-GET verb
	static const QByteArray GetVerb;
	//! A constant for the HTTP-POST verb
	static const QByteArray PostVerb;
	//! A constant for the HTTP-PUT verb
	static const QByteArray PutVerb;
	//! A constant for the HTTP-DELETE verb
	static const QByteArray DeleteVerb;
	//! A constant for the HTTP-PATCH verb
	static const QByteArray PatchVerb;

	~RestClass();

	//! Returns the rest client this class operates with
	RestClient *client() const;
	//! Creates a new rest class based on this one for the given path and parent
	RestClass *subClass(const QString &path, QObject *parent = nullptr);

	//general calls (json based)
	//! @{
	//! @brief Performs a API call of the given verb with JSON data
	RestReply *callJson(QByteArray verb, const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	RestReply *callJson(QByteArray verb, const QString &methodPath, QJsonObject body, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	RestReply *callJson(QByteArray verb, const QString &methodPath, QJsonArray body, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	RestReply *callJson(QByteArray verb, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	RestReply *callJson(QByteArray verb, QJsonObject body, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	RestReply *callJson(QByteArray verb, QJsonArray body, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	RestReply *callJson(QByteArray verb, const QUrl &relativeUrl, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	RestReply *callJson(QByteArray verb, const QUrl &relativeUrl, QJsonObject body, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	RestReply *callJson(QByteArray verb, const QUrl &relativeUrl, QJsonArray body, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	//! @}

	//general calls
	//! @{
	//! @brief Performs a API call of the given verb with generic objects
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *call(QByteArray verb, const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *call(QByteArray verb, const QString &methodPath, RO body, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *call(QByteArray verb, const QString &methodPath, QList<RO> body, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *call(QByteArray verb, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *call(QByteArray verb, RO body, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *call(QByteArray verb, QList<RO> body, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *call(QByteArray verb, const QUrl &relativeUrl, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *call(QByteArray verb, const QUrl &relativeUrl, RO body, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *call(QByteArray verb, const QUrl &relativeUrl, QList<RO> body, const QVariantHash &parameters = {}, const HeaderHash &headers = {});
	//! @}

	//the following methods are simply shortcuts, and thus inlinied
	//! @{
	//! @brief Performs GET-request with generic objects
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *get(const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(GetVerb, methodPath, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *get(const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(GetVerb, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *get(const QUrl &relativeUrl, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(GetVerb, relativeUrl, parameters, headers);
	}
	//! @}
	//! @{
	//! @brief Performs POST-request with generic objects
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *post(const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PostVerb, methodPath, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *post(const QString &methodPath, RO body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PostVerb, methodPath, body, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *post(const QString &methodPath, QList<RO> body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PostVerb, methodPath, body, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *post(const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PostVerb, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *post(RO body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PostVerb, body, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *post(QList<RO> body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PostVerb, body, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *post(const QUrl &relativeUrl, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PostVerb, relativeUrl, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *post(const QUrl &relativeUrl, RO body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PostVerb, relativeUrl, body, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *post(const QUrl &relativeUrl, QList<RO> body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PostVerb, relativeUrl, body, parameters, headers);
	}
	//! @}
	//! @{
	//! @brief Performs PUT-request with generic objects
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *put(const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PutVerb, methodPath, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *put(const QString &methodPath, RO body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PutVerb, methodPath, body, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *put(const QString &methodPath, QList<RO> body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PutVerb, methodPath, body, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *put(const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PutVerb, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *put(RO body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PutVerb, body, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *put(QList<RO> body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PutVerb, body, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *put(const QUrl &relativeUrl, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PutVerb, relativeUrl, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *put(const QUrl &relativeUrl, RO body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PutVerb, relativeUrl, body, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *put(const QUrl &relativeUrl, QList<RO> body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PutVerb, relativeUrl, body, parameters, headers);
	}
	//! @}
	//! @{
	//! @brief Performs DELETE-request with generic objects
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *deleteResource(const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(DeleteVerb, methodPath, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *deleteResource(const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(DeleteVerb, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *deleteResource(const QUrl &relativeUrl, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(DeleteVerb, relativeUrl, parameters, headers);
	}
	//! @}
	//! @{
	//! @brief Performs PATCH-request with generic objects
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *patch(const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PatchVerb, methodPath, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *patch(const QString &methodPath, RO body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PatchVerb, methodPath, body, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *patch(const QString &methodPath, QList<RO> body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PatchVerb, methodPath, body, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *patch(const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PatchVerb, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *patch(RO body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PatchVerb, body, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *patch(QList<RO> body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PatchVerb, body, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *patch(const QUrl &relativeUrl, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PatchVerb, relativeUrl, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *patch(const QUrl &relativeUrl, RO body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PatchVerb, relativeUrl, body, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *patch(const QUrl &relativeUrl, QList<RO> body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) {
		return call<DT, ET>(PatchVerb, relativeUrl, body, parameters, headers);
	}
	//! @}

	//! Creates a request builder for this class
	virtual RequestBuilder builder() const;

	//! @{
	//! @brief A generic method to concatenate parameters into a QVariantHash
	static inline QVariantHash concatParams() {return {};}
	template<typename... Args>
	static QVariantHash concatParams(QString key, QVariant value, Args... parameters);
	//! @}

private:
	QScopedPointer<RestClassPrivate> d;

	explicit RestClass(RestClient *client, QStringList subPath, QObject *parent);

	QNetworkReply *create(QByteArray verb, const QString &methodPath, const QVariantHash &parameters, const HeaderHash &headers);
	QNetworkReply *create(QByteArray verb, const QString &methodPath, QJsonObject body, const QVariantHash &parameters, const HeaderHash &headers);
	QNetworkReply *create(QByteArray verb, const QString &methodPath, QJsonArray body, const QVariantHash &parameters, const HeaderHash &headers);
	QNetworkReply *create(QByteArray verb, const QVariantHash &parameters, const HeaderHash &headers);
	QNetworkReply *create(QByteArray verb, QJsonObject body, const QVariantHash &parameters, const HeaderHash &headers);
	QNetworkReply *create(QByteArray verb, QJsonArray body, const QVariantHash &parameters, const HeaderHash &headers);
	QNetworkReply *create(QByteArray verb, const QUrl &relativeUrl, const QVariantHash &parameters, const HeaderHash &headers);
	QNetworkReply *create(QByteArray verb, const QUrl &relativeUrl, QJsonObject body, const QVariantHash &parameters, const HeaderHash &headers);
	QNetworkReply *create(QByteArray verb, const QUrl &relativeUrl, QJsonArray body, const QVariantHash &parameters, const HeaderHash &headers);
};

//! Short macro for RestClass::concatParams(), to make the call shorter
#define CONCAT_PARAMS QtRestClient::RestClass::concatParams

// ------------- Generic Implementation -------------

template<typename DT, typename ET>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, const QString &methodPath, const QVariantHash &parameters, const HeaderHash &headers)
{
	return new GenericRestReply<DT, ET>(create(verb,
											   methodPath,
											   parameters,
											   headers),
										client(),
										this);
}

template<typename DT, typename ET, typename RO>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, const QString &methodPath, RO body, const QVariantHash &parameters, const HeaderHash &headers)
{
	static_assert(MetaComponent<RO>::is_meta::value, "RO must inherit QObject or have Q_GADGET!");
	return new GenericRestReply<DT, ET>(create(verb,
											   methodPath,
											   client()->serializer()->serialize(body),
											   parameters,
											   headers),
										client(),
										this);
}

template<typename DT, typename ET, typename RO>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, const QString &methodPath, QList<RO> body, const QVariantHash &parameters, const HeaderHash &headers)
{
	static_assert(MetaComponent<RO>::is_meta::value, "RO must inherit QObject or have Q_GADGET!");
	return new GenericRestReply<DT, ET>(create(verb,
											   methodPath,
											   client()->serializer()->serialize(body),
											   parameters,
											   headers),
										client(),
										this);
}

template<typename DT, typename ET>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, const QVariantHash &parameters, const HeaderHash &headers)
{
	return new GenericRestReply<DT, ET>(create(verb,
											   parameters,
											   headers),
										client(),
										this);
}

template<typename DT, typename ET, typename RO>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, RO body, const QVariantHash &parameters, const HeaderHash &headers)
{
	static_assert(MetaComponent<RO>::is_meta::value, "RO must inherit QObject or have Q_GADGET!");
	return new GenericRestReply<DT, ET>(create(verb,
											   client()->serializer()->serialize(body),
											   parameters,
											   headers),
										client(),
										this);
}

template<typename DT, typename ET, typename RO>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, QList<RO> body, const QVariantHash &parameters, const HeaderHash &headers)
{
	static_assert(MetaComponent<RO>::is_meta::value, "RO must inherit QObject or have Q_GADGET!");
	return new GenericRestReply<DT, ET>(create(verb,
											   client()->serializer()->serialize(body),
											   parameters,
											   headers),
										client(),
										this);
}

template<typename DT, typename ET>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, const QUrl &relativeUrl, const QVariantHash &parameters, const HeaderHash &headers)
{
	return new GenericRestReply<DT, ET>(create(verb,
											   relativeUrl,
											   parameters,
											   headers),
										client(),
										this);
}

template<typename DT, typename ET, typename RO>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, const QUrl &relativeUrl, RO body, const QVariantHash &parameters, const HeaderHash &headers)
{
	static_assert(MetaComponent<RO>::is_meta::value, "RO must inherit QObject or have Q_GADGET!");
	return new GenericRestReply<DT, ET>(create(verb,
											   relativeUrl,
											   client()->serializer()->serialize(body),
											   parameters,
											   headers),
										client(),
										this);
}

template<typename DT, typename ET, typename RO>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, const QUrl &relativeUrl, QList<RO> body, const QVariantHash &parameters, const HeaderHash &headers)
{
	static_assert(MetaComponent<RO>::is_meta::value, "RO must inherit QObject or have Q_GADGET!");
	return new GenericRestReply<DT, ET>(create(verb,
											   relativeUrl,
											   client()->serializer()->serialize(body),
											   parameters,
											   headers),
										client(),
										this);
}

template<typename... Args>
QVariantHash RestClass::concatParams(QString key, QVariant value, Args... parameters)
{
	auto hash = concatParams(parameters...);
	hash.insert(key, value);
	return hash;
}

}

#endif // QTRESTCLIENT_RESTCLASS_H
//! @file
