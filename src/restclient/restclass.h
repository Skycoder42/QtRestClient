#ifndef QTRESTCLIENT_RESTCLASS_H
#define QTRESTCLIENT_RESTCLASS_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/requestbuilder.h"
#include "QtRestClient/restreply.h"
#include "QtRestClient/restclient.h"

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
#include "QtRestClient/genericrestreply.h"
#endif

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
	//! A constant for the HTTP-HEAD verb
	static const QByteArray HeadVerb;

	~RestClass() override;

	//! Returns the rest client this class operates with
	RestClient *client() const;
	//! Creates a new rest class based on this one for the given path and parent
	RestClass *subClass(const QString &path, QObject *parent = nullptr);

	//general calls (json based)
	//! @{
	//! @brief Performs a API call of the given verb with JSON data
	RestReply *callJson(const QByteArray &verb, const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash &headers = {}, bool paramsAsBody = false) const;
	RestReply *callJson(const QByteArray &verb, const QString &methodPath, const QJsonObject &body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const;
	RestReply *callJson(const QByteArray &verb, const QString &methodPath, const QJsonArray &body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const;

	RestReply *callJson(const QByteArray &verb, const QVariantHash &parameters = {}, const HeaderHash &headers = {}, bool paramsAsBody = false) const;
	RestReply *callJson(const QByteArray &verb, const QJsonObject &body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const;
	RestReply *callJson(const QByteArray &verb, const QJsonArray &body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const;

	RestReply *callJson(const QByteArray &verb, const QUrl &relativeUrl, const QVariantHash &parameters = {}, const HeaderHash &headers = {}, bool paramsAsBody = false) const;
	RestReply *callJson(const QByteArray &verb, const QUrl &relativeUrl, const QJsonObject &body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const;
	RestReply *callJson(const QByteArray &verb, const QUrl &relativeUrl, const QJsonArray &body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const;
	//! @}

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	//general calls
	//! @{
	//! @brief Performs a API call of the given verb with generic objects
	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *call(const QByteArray &verb, const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash &headers = {}, bool paramsAsBody = false) const;
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *call(const QByteArray &verb, const QString &methodPath, const RO &body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const;
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *call(const QByteArray &verb, const QString &methodPath, const QList<RO> &body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const;

	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *call(const QByteArray &verb, const QVariantHash &parameters = {}, const HeaderHash &headers = {}, bool paramsAsBody = false) const;
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *call(const QByteArray &verb, const RO &body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const;
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *call(const QByteArray &verb, const QList<RO> &body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const;

	template<typename DT = QObject*, typename ET = QObject*>
	GenericRestReply<DT, ET> *call(const QByteArray &verb, const QUrl &relativeUrl, const QVariantHash &parameters = {}, const HeaderHash &headers = {}, bool paramsAsBody = false) const;
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *call(const QByteArray &verb, const QUrl &relativeUrl, const RO &body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const;
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	GenericRestReply<DT, ET> *call(const QByteArray &verb, const QUrl &relativeUrl, const QList<RO> &body, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const;
	//! @}

	//the following methods are simply shortcuts, and thus inlinied
	//! @{
	//! @brief Performs GET-request with generic objects
	template<typename DT = QObject*, typename ET = QObject*>
	inline GenericRestReply<DT, ET> *get(const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const {
		return call<DT, ET>(GetVerb, methodPath, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*>
	inline GenericRestReply<DT, ET> *get(const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const {
		return call<DT, ET>(GetVerb, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*>
	inline GenericRestReply<DT, ET> *get(const QUrl &relativeUrl, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const {
		return call<DT, ET>(GetVerb, relativeUrl, parameters, headers);
	}
	//! @}

	//! @{
	//! @brief Performs POST-request with generic objects
	template<typename DT = QObject*, typename ET = QObject*>
	inline GenericRestReply<DT, ET> *post(const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PostVerb, methodPath, parameters, headers, true);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *post(const QString &methodPath, const RO &body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PostVerb, methodPath, body, {}, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *post(const QString &methodPath, const QList<RO> &body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PostVerb, methodPath, body, {}, headers);
	}

	template<typename DT = QObject*, typename ET = QObject*>
	inline GenericRestReply<DT, ET> *post(const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PostVerb, parameters, headers, true);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *post(const RO &body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PostVerb, body, {}, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *post(const QList<RO> &body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PostVerb, body, {}, headers);
	}

	template<typename DT = QObject*, typename ET = QObject*>
	inline GenericRestReply<DT, ET> *post(const QUrl &relativeUrl, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PostVerb, relativeUrl, parameters, headers, true);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *post(const QUrl &relativeUrl, const RO &body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PostVerb, relativeUrl, body, {}, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *post(const QUrl &relativeUrl, const QList<RO> &body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PostVerb, relativeUrl, body, {}, headers);
	}
	//! @}

	//! @{
	//! @brief Performs PUT-request with generic objects
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *put(const QString &methodPath, const RO &body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PutVerb, methodPath, body, {}, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *put(const QString &methodPath, const QList<RO> &body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PutVerb, methodPath, body, {}, headers);
	}

	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *put(const RO &body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PutVerb, body, {}, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *put(const QList<RO> &body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PutVerb, body, {}, headers);
	}

	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *put(const QUrl &relativeUrl, const RO &body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PutVerb, relativeUrl, body, {}, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *put(const QUrl &relativeUrl, const QList<RO>& body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PutVerb, relativeUrl, body, {}, headers);
	}
	//! @}

	//! @{
	//! @brief Performs DELETE-request with generic objects
	template<typename DT = QObject*, typename ET = QObject*>
	inline GenericRestReply<DT, ET> *deleteResource(const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const {
		return call<DT, ET>(DeleteVerb, methodPath, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*>
	inline GenericRestReply<DT, ET> *deleteResource(const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const {
		return call<DT, ET>(DeleteVerb, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*>
	inline GenericRestReply<DT, ET> *deleteResource(const QUrl &relativeUrl, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const {
		return call<DT, ET>(DeleteVerb, relativeUrl, parameters, headers);
	}
	//! @}

	//! @{
	//! @brief Performs PATCH-request with generic objects
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *patch(const QString &methodPath, const RO &body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PatchVerb, methodPath, body, {}, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *patch(const QString &methodPath, const QList<RO> &body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PatchVerb, methodPath, body, {}, headers);
	}

	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *patch(const RO &body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PatchVerb, body, {}, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *patch(const QList<RO> &body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PatchVerb, body, {}, headers);
	}

	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *patch(const QUrl &relativeUrl, const RO &body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PatchVerb, relativeUrl, body, {}, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*, typename RO = QObject*>
	inline GenericRestReply<DT, ET> *patch(const QUrl &relativeUrl, const QList<RO> &body, const HeaderHash &headers = {}) const {
		return call<DT, ET>(PatchVerb, relativeUrl, body, {}, headers);
	}
	//! @}

	//! @{
	//! @brief Performs HEAD-request with generic objects
	template<typename DT = QObject*, typename ET = QObject*>
	inline GenericRestReply<DT, ET> *head(const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const {
		return call<DT, ET>(HeadVerb, methodPath, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*>
	inline GenericRestReply<DT, ET> *head(const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const {
		return call<DT, ET>(HeadVerb, parameters, headers);
	}
	template<typename DT = QObject*, typename ET = QObject*>
	inline GenericRestReply<DT, ET> *head(const QUrl &relativeUrl, const QVariantHash &parameters = {}, const HeaderHash &headers = {}) const {
		return call<DT, ET>(HeadVerb, relativeUrl, parameters, headers);
	}
	//! @}
#endif

	//! Creates a request builder for this class
	virtual RequestBuilder builder() const;

	//! @{
	//! @brief A generic method to concatenate parameters into a QVariantHash
	static inline QVariantHash concatParams() {return {};}
	template<typename... Args>
	static QVariantHash concatParams(const QString &key, const QVariant &value, Args... parameters);
	//! @}

protected:
	//! @private
	explicit RestClass(RestClient *client, QStringList subPath, QObject *parent);

private:
	QScopedPointer<RestClassPrivate> d;

	QNetworkReply *create(const QByteArray &verb, const QString &methodPath, const QVariantHash &parameters, const HeaderHash &headers, bool paramsAsBody) const;
	QNetworkReply *create(const QByteArray &verb, const QString &methodPath, const QJsonObject &body, const QVariantHash &parameters, const HeaderHash &headers) const;
	QNetworkReply *create(const QByteArray &verb, const QString &methodPath, const QJsonArray &body, const QVariantHash &parameters, const HeaderHash &headers) const;
	QNetworkReply *create(const QByteArray &verb, const QVariantHash &parameters, const HeaderHash &headers, bool paramsAsBody) const;
	QNetworkReply *create(const QByteArray &verb, const QJsonObject &body, const QVariantHash &parameters, const HeaderHash &headers) const;
	QNetworkReply *create(const QByteArray &verb, const QJsonArray &body, const QVariantHash &parameters, const HeaderHash &headers) const;
	QNetworkReply *create(const QByteArray &verb, const QUrl &relativeUrl, const QVariantHash &parameters, const HeaderHash &headers, bool paramsAsBody) const;
	QNetworkReply *create(const QByteArray &verb, const QUrl &relativeUrl, const QJsonObject &body, const QVariantHash &parameters, const HeaderHash &headers) const;
	QNetworkReply *create(const QByteArray &verb, const QUrl &relativeUrl, const QJsonArray &body, const QVariantHash &parameters, const HeaderHash &headers) const;
};

//! Short macro for RestClass::concatParams(), to make the call shorter
#define CONCAT_PARAMS QtRestClient::RestClass::concatParams

// ------------- Generic Implementation -------------

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
template<typename DT, typename ET>
GenericRestReply<DT, ET> *RestClass::call(const QByteArray &verb, const QString &methodPath, const QVariantHash &parameters, const HeaderHash &headers, bool paramsAsBody) const
{
	return new GenericRestReply<DT, ET>(create(verb,
											   methodPath,
											   parameters,
											   headers,
											   paramsAsBody),
										client(),
										nullptr);
}

template<typename DT, typename ET, typename RO>
GenericRestReply<DT, ET> *RestClass::call(const QByteArray &verb, const QString &methodPath, const RO &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	static_assert(MetaComponent<RO>::value, "RO must inherit QObject or have Q_GADGET!");
	return new GenericRestReply<DT, ET>(create(verb,
											   methodPath,
											   client()->serializer()->serialize(body),
											   parameters,
											   headers),
										client(),
										nullptr);
}

template<typename DT, typename ET, typename RO>
GenericRestReply<DT, ET> *RestClass::call(const QByteArray &verb, const QString &methodPath, const QList<RO> &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	static_assert(MetaComponent<RO>::value, "RO must inherit QObject or have Q_GADGET!");
	return new GenericRestReply<DT, ET>(create(verb,
											   methodPath,
											   client()->serializer()->serialize(body),
											   parameters,
											   headers),
										client(),
										nullptr);
}

template<typename DT, typename ET>
GenericRestReply<DT, ET> *RestClass::call(const QByteArray &verb, const QVariantHash &parameters, const HeaderHash &headers, bool paramsAsBody) const
{
	return new GenericRestReply<DT, ET>(create(verb,
											   parameters,
											   headers,
											   paramsAsBody),
										client(),
										nullptr);
}

template<typename DT, typename ET, typename RO>
GenericRestReply<DT, ET> *RestClass::call(const QByteArray &verb, const RO &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	static_assert(MetaComponent<RO>::value, "RO must inherit QObject or have Q_GADGET!");
	return new GenericRestReply<DT, ET>(create(verb,
											   client()->serializer()->serialize(body),
											   parameters,
											   headers),
										client(),
										nullptr);
}

template<typename DT, typename ET, typename RO>
GenericRestReply<DT, ET> *RestClass::call(const QByteArray &verb, const QList<RO> &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	static_assert(MetaComponent<RO>::value, "RO must inherit QObject or have Q_GADGET!");
	return new GenericRestReply<DT, ET>(create(verb,
											   client()->serializer()->serialize(body),
											   parameters,
											   headers),
										client(),
										nullptr);
}

template<typename DT, typename ET>
GenericRestReply<DT, ET> *RestClass::call(const QByteArray &verb, const QUrl &relativeUrl, const QVariantHash &parameters, const HeaderHash &headers, bool paramsAsBody) const
{
	return new GenericRestReply<DT, ET>(create(verb,
											   relativeUrl,
											   parameters,
											   headers,
											   paramsAsBody),
										client(),
										nullptr);
}

template<typename DT, typename ET, typename RO>
GenericRestReply<DT, ET> *RestClass::call(const QByteArray &verb, const QUrl &relativeUrl, const RO &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	static_assert(MetaComponent<RO>::value, "RO must inherit QObject or have Q_GADGET!");
	return new GenericRestReply<DT, ET>(create(verb,
											   relativeUrl,
											   client()->serializer()->serialize(body),
											   parameters,
											   headers),
										client(),
										nullptr);
}

template<typename DT, typename ET, typename RO>
GenericRestReply<DT, ET> *RestClass::call(const QByteArray &verb, const QUrl &relativeUrl, const QList<RO> &body, const QVariantHash &parameters, const HeaderHash &headers) const
{
	static_assert(MetaComponent<RO>::value, "RO must inherit QObject or have Q_GADGET!");
	return new GenericRestReply<DT, ET>(create(verb,
											   relativeUrl,
											   client()->serializer()->serialize(body),
											   parameters,
											   headers),
										client(),
										nullptr);
}
#endif

template<typename... Args>
QVariantHash RestClass::concatParams(const QString &key, const QVariant &value, Args... parameters)
{
	auto hash = concatParams(parameters...);
	hash.insert(key, value);
	return hash;
}

}

#endif // QTRESTCLIENT_RESTCLASS_H
//! @file
