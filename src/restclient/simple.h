#ifndef SIMPLEBASE_H
#define SIMPLEBASE_H

#include "QtRestClient/qrestclient_global.h"
#include "QtRestClient/genericrestreply.h"
#include "QtRestClient/restclass.h"

#include <QtCore/qpointer.h>
#include <functional>

namespace QtRestClient {

template <typename T, typename = void>
class Simple {};

template<typename T>
class Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type> : public QObject
{
public:
	Simple(QObject *parent = nullptr);

	virtual QUrl extensionHref() const = 0;
	bool hasExtension() const;
	T *currentExtended() const;

	template<typename ET = QObject*>
	GenericRestReply<T*, ET> *extend(RestClient *client);

	template<typename ET = QObject*>
	void extend(RestClient *client,
				std::function<void(T*, bool)> extensionHandler,
				std::function<void(RestReply*, QString, int, RestReply::ErrorType)> errorHandler = {},
				std::function<QString(ET, int)> failureTransformer = {});

private:
	QPointer<T> cExt;
};

template<typename T>
class Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>
{
public:
	Simple();

	virtual QUrl extensionHref() const = 0;
	bool hasExtension() const;
	T currentExtended() const;

	template<typename ET = QObject*>
	GenericRestReply<T, ET> *extend(RestClient *client);

	template<typename ET = QObject*>
	void extend(RestClient *client,
				std::function<void(T, bool)> extensionHandler,
				std::function<void(RestReply*, QString, int, RestReply::ErrorType)> errorHandler = {},
				std::function<QString(ET, int)> failureTransformer = {});

private:
	T cExt;
};

}

#define QTRESTCLIENT_EXT_HREF_PROP(x) \
	inline QUrl extensionHref() const override { \
		return property(x).toUrl();\
	}

// ------------- Generic Implementation object -------------

namespace QtRestClient {

template<typename T>
Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>::Simple(QObject *parent) :
	QObject(parent)
{}

template<typename T>
bool Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>::hasExtension() const
{
	return extensionHref().isValid();
}

template<typename T>
T *Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>::currentExtended() const
{
	return cExt;
}

template<typename T>
template<typename ET>
GenericRestReply<T*, ET> *Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>::extend(RestClient *client)
{
	return client->rootClass()->get<T*, ET>(extensionHref())
			->onSucceeded([=](RestReply *, int, T *data){
				cExt = data;
			});
}

template<typename T>
template<typename ET>
void Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>::extend(RestClient *client, std::function<void (T *, bool)> extensionHandler, std::function<void (RestReply *, QString, int, RestReply::ErrorType)> errorHandler, std::function<QString (ET, int)> failureTransformer)
{
	if(cExt)
		extensionHandler(cExt, false);
	else {
		client->rootClass()->get<T*, ET>(extensionHref())
				->enableAutoDelete()
				->onSucceeded([=](RestReply *, int, T *data){
					cExt = data;
					extensionHandler(data, true);
				})
				->onAllErrors(errorHandler, failureTransformer);
	}
}

// ------------- Generic Implementation gadget -------------

template<typename T>
Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::Simple() {}

template<typename T>
bool Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::hasExtension() const
{
	return extensionHref().isValid();
}

template<typename T>
T Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::currentExtended() const
{
	return cExt;
}

template<typename T>
template<typename ET>
GenericRestReply<T, ET> *Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::extend(RestClient *client)
{
	return client->rootClass()->get<T, ET>(extensionHref())
			->onSucceeded([=](RestReply *, int, T data){
				cExt = data;
			});
}

template<typename T>
template<typename ET>
void Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::extend(RestClient *client, std::function<void (T, bool)> extensionHandler, std::function<void (RestReply *, QString, int, RestReply::ErrorType)> errorHandler, std::function<QString (ET, int)> failureTransformer)
{
	if(cExt)
		extensionHandler(cExt, false);
	else {
		client->rootClass()->get<T, ET>(extensionHref())
				->enableAutoDelete()
				->onSucceeded([=](RestReply *, int, T data){
					cExt = data;
					extensionHandler(data, true);
				})
				->onAllErrors(errorHandler, failureTransformer);
	}
}

}

#endif // SIMPLEBASE_H
