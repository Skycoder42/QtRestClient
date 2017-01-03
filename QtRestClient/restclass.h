#ifndef RESTCLASS_H
#define RESTCLASS_H

#include "qtrestclient_global.h"
#include "requestbuilder.h"
#include "restreply.h"
#include "genericrestreply.h"
#include "restclient.h"

#include <QObject>

namespace QtRestClient {

class RestClassPrivate;
class QTRESTCLIENTSHARED_EXPORT RestClass : public QObject
{
	Q_OBJECT
	friend class RestClient;
	friend class RestClientPrivate;

public:
	static const QByteArray GetVerb;
	static const QByteArray PostVerb;
	static const QByteArray PutVerb;
	static const QByteArray DeleteVerb;
	static const QByteArray PatchVerb;

	~RestClass();

	RestClient *client() const;
	RestClass *subClass(const QString &path, QObject *parent = nullptr);

	//general calls
	RestReply *callJson(QByteArray verb, const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
	RestReply *callJson(QByteArray verb, const QString &methodPath, QJsonObject body, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
	RestReply *callJson(QByteArray verb, const QString &methodPath, QJsonArray body, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
	RestReply *callJson(QByteArray verb, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
	RestReply *callJson(QByteArray verb, QJsonObject body, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
	RestReply *callJson(QByteArray verb, QJsonArray body, const QVariantHash &parameters = {}, const HeaderHash & headers = {});

	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *call(QByteArray verb, const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *call(QByteArray verb, const QString &methodPath, RestObject *body, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
	template<typename DT = RestObject, typename ET = RestObject, typename RO = RestObject>
	GenericRestReply<DT, ET> *call(QByteArray verb, const QString &methodPath, QList<RO*> body, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *call(QByteArray verb, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *call(QByteArray verb, RestObject *body, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
	template<typename DT = RestObject, typename ET = RestObject, typename RO = RestObject>
	GenericRestReply<DT, ET> *call(QByteArray verb, QList<RO*> body, const QVariantHash &parameters = {}, const HeaderHash & headers = {});

	//the following methods are simply shortcuts, and thus inlinied
	//get
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *get(const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(GetVerb, methodPath, parameters, headers);
	}
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *get(const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(GetVerb, parameters, headers);
	}
	//post
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *post(const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PostVerb, methodPath, parameters, headers);
	}
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *post(const QString &methodPath, RestObject *body, const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PostVerb, methodPath, body, parameters, headers);
	}
	template<typename DT = RestObject, typename ET = RestObject, typename RO = RestObject>
	GenericRestReply<DT, ET> *post(const QString &methodPath, QList<RO*> body, const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PostVerb, methodPath, body, parameters, headers);
	}
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *post(const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PostVerb, parameters, headers);
	}
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *post(RestObject *body, const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PostVerb, body, parameters, headers);
	}
	template<typename DT = RestObject, typename ET = RestObject, typename RO = RestObject>
	GenericRestReply<DT, ET> *post(QList<RO*> body, const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PostVerb, body, parameters, headers);
	}
	//put
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *put(const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PutVerb, methodPath, parameters, headers);
	}
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *put(const QString &methodPath, RestObject *body, const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PutVerb, methodPath, body, parameters, headers);
	}
	template<typename DT = RestObject, typename ET = RestObject, typename RO = RestObject>
	GenericRestReply<DT, ET> *put(const QString &methodPath, QList<RO*> body, const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PutVerb, methodPath, body, parameters, headers);
	}
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *put(const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PutVerb, parameters, headers);
	}
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *put(RestObject *body, const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PutVerb, body, parameters, headers);
	}
	template<typename DT = RestObject, typename ET = RestObject, typename RO = RestObject>
	GenericRestReply<DT, ET> *put(QList<RO*> body, const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PutVerb, body, parameters, headers);
	}
	//delete
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *deleteResource(const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(DeleteVerb, methodPath, parameters, headers);
	}
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *deleteResource(const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(DeleteVerb, parameters, headers);
	}
	//patch
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *patch(const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PatchVerb, methodPath, parameters, headers);
	}
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *patch(const QString &methodPath, RestObject *body, const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PatchVerb, methodPath, body, parameters, headers);
	}
	template<typename DT = RestObject, typename ET = RestObject, typename RO = RestObject>
	GenericRestReply<DT, ET> *patch(const QString &methodPath, QList<RO*> body, const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PatchVerb, methodPath, body, parameters, headers);
	}
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *patch(const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PatchVerb, parameters, headers);
	}
	template<typename DT = RestObject, typename ET = RestObject>
	GenericRestReply<DT, ET> *patch(RestObject *body, const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PatchVerb, body, parameters, headers);
	}
	template<typename DT = RestObject, typename ET = RestObject, typename RO = RestObject>
	GenericRestReply<DT, ET> *patch(QList<RO*> body, const QVariantHash &parameters = {}, const HeaderHash & headers = {}) {
		return call<DT, ET>(PatchVerb, body, parameters, headers);
	}

	RequestBuilder builder() const;

	static inline QVariantHash concatParameters() {return {};}
	template<typename... Args>
	static QVariantHash concatParameters(QString key, QVariant value, Args... parameters);

private:
	QScopedPointer<RestClassPrivate> d_ptr;

	explicit RestClass(RestClient *client, QStringList subPath, QObject *parent);

	QNetworkReply *create(QByteArray verb, const QString &methodPath, const QVariantHash &parameters, const HeaderHash & headers);
	QNetworkReply *create(QByteArray verb, const QString &methodPath, QJsonObject body, const QVariantHash &parameters, const HeaderHash & headers);
	QNetworkReply *create(QByteArray verb, const QString &methodPath, QJsonArray body, const QVariantHash &parameters, const HeaderHash & headers);
	QNetworkReply *create(QByteArray verb, const QVariantHash &parameters, const HeaderHash & headers);
	QNetworkReply *create(QByteArray verb, QJsonObject body, const QVariantHash &parameters, const HeaderHash & headers);
	QNetworkReply *create(QByteArray verb, QJsonArray body, const QVariantHash &parameters, const HeaderHash & headers);
};

// ------------- Generic Implementation -------------

template<typename DT, typename ET>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, const QString &methodPath, const QVariantHash &parameters, const HeaderHash & headers)
{
	return new GenericRestReply<DT, ET>(create(verb,
											   methodPath,
											   parameters,
											   headers),
										client()->serializer(),
										this);
}

template<typename DT, typename ET>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, const QString &methodPath, RestObject *body, const QVariantHash &parameters, const HeaderHash & headers)
{
	return new GenericRestReply<DT, ET>(create(verb,
											   methodPath,
											   client()->serializer()->serialize(body),
											   parameters,
											   headers),
										client()->serializer(),
										this);
}

template<typename DT, typename ET, typename RO>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, const QString &methodPath, QList<RO*> body, const QVariantHash &parameters, const HeaderHash & headers)
{
	return new GenericRestReply<DT, ET>(create(verb,
											   methodPath,
											   client()->serializer()->serialize(body),
											   parameters,
											   headers),
										client()->serializer(),
										this);
}

template<typename DT, typename ET>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, const QVariantHash &parameters, const HeaderHash & headers)
{
	return new GenericRestReply<DT, ET>(create(verb,
											   parameters,
											   headers),
										client()->serializer(),
										this);
}

template<typename DT, typename ET>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, RestObject *body, const QVariantHash &parameters, const HeaderHash & headers)
{
	return new GenericRestReply<DT, ET>(create(verb,
											   client()->serializer()->serialize(body),
											   parameters,
											   headers),
										client()->serializer(),
										this);
}

template<typename DT, typename ET, typename RO>
GenericRestReply<DT, ET> *RestClass::call(QByteArray verb, QList<RO *> body, const QVariantHash &parameters, const HeaderHash &headers)
{
	return new GenericRestReply<DT, ET>(create(verb,
											   client()->serializer()->serialize(body),
											   parameters,
											   headers),
										client()->serializer(),
										this);
}

template<typename... Args>
static QVariantHash RestClass::concatParameters(QString key, QVariant value, Args... parameters)
{
	auto hash = concatParameters(parameters...);
	hash.insert(key, value);
	return hash;
}

}

#endif // RESTCLASS_H
