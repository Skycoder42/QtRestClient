#ifndef SIMPLEBASE_H
#define SIMPLEBASE_H

#include "qtrestclient_global.h"

#include "genericrestreply.h"
#include "restclass.h"

#include <QPointer>
#include <functional>

namespace QtRestClient {

template<typename T>
class Simple : public RestObject
{
	static_assert(std::is_base_of<RestObject, T>::value, "T must inherit RestObject!");
public:
	Simple(QObject *parent = nullptr);

	virtual QUrl extensionHref() const = 0;
	bool hasExtension() const;
	T *currentExtended() const;

	template<typename ET = RestObject>
	GenericRestReply<T, ET> *extend(RestClient *client);

	template<typename ET = RestObject>
	void onExtended(RestClient *client,
					std::function<void(T*, bool)> extensionHandler,
					std::function<void(RestReply*, int, ET*)> failureHandler = {},
					std::function<void(RestReply*, QString, int, RestReply::ErrorType)> errorHandler = {},
					std::function<void(RestReply*, SerializerException &)> exceptionHandler = {});

private:
	QPointer<T> cExt;
};

}

#define QTRESTCLIENT_EXT_HREF_PROP(x) \
	inline QUrl extensionHref() const override { \
		return property(x).toUrl();\
	}

// ------------- Generic Implementation -------------

namespace QtRestClient {

template<typename T>
Simple<T>::Simple(QObject *parent) :
	RestObject(parent)
{}

template<typename T>
bool Simple<T>::hasExtension() const
{
	return extensionHref().isValid();
}

template<typename T>
typename T *Simple<T>::currentExtended() const
{
	return cExt;
}

template<typename T>
template<typename ET>
GenericRestReply<T, ET> *Simple<T>::extend(RestClient *client)
{
	return client->rootClass()->get<T, ET>(extensionHref())
			->onSucceeded([=](RestReply *, int, T *data){
				cExt = data;
			});
}

template<typename T>
template<typename ET>
void Simple<T>::onExtended(RestClient *client, std::function<void (T *, bool)> extensionHandler, std::function<void (RestReply *, int, ET *)> failureHandler, std::function<void (RestReply *, QString, int, RestReply::ErrorType)> errorHandler, std::function<void (RestReply *, SerializerException &)> exceptionHandler)
{
	if(cExt)
		extensionHandler(cExt, false);
	else {
		client->rootClass()->get<T, ET>(extensionHref())
				->enableAutoDelete()
				->onSucceeded([=](RestReply *, int, T *data){
					cExt = data;
					extensionHandler(data, true);
				})
				->onFailed(failureHandler)
				->onError(errorHandler)
				->onSerializeException(exceptionHandler);
	}
}

}

#endif // SIMPLEBASE_H
