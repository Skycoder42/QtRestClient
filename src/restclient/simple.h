#ifndef QTRESTCLIENT_SIMPLEBASE_H
#define QTRESTCLIENT_SIMPLEBASE_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/genericrestreply.h"
#include "QtRestClient/restclass.h"

#include <QtCore/qpointer.h>
#include <QtCore/qsharedpointer.h>
#include <functional>

namespace QtRestClient {

//! A base class to create a simply type version for a normal one. Check The QObject* or Q_GADGET version
template <typename T, typename = void>
class Simple {};

//! A base class to create a simply type version for a normal one. QObject* version
template<typename T>
class Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type> : public QObject
{
public:
	//! Constructor
	Simple(QObject *parent = nullptr);

	//! Returns the link to the extended version of this object. @sa #QTRESTCLIENT_EXT_HREF_PROP
	virtual QUrl extensionHref() const = 0;
	//! Returns true, if the object has an extension
	bool hasExtension() const;
	//! Returns true, if the object has already been extended
	bool isExtended() const;
	//! @brief Returns a reference to the current extension.
	//! @returns The current extension, or `nullptr`, if not extended
	//! @details You will have to extend the object, before you can use this. Please note that the
	//! simple object does **note** own it's extension. It's safe to delete it
	T *currentExtended() const;

	//! @brief Performs an extension request and returns the reply to the request
	//! @param client The rest client to be used to create the network request
	//! @returns The reply, or `nullptr` if no extension exists
	template<typename ET = QObject*>
	GenericRestReply<T*, ET> *extend(RestClient *client);

	//! @brief Extends the object by using a handler
	//! @param client The rest client to be used to create the network request
	//! @param extensionHandler The handler to be called with the extended object
	//! @param errorHandler Will be passed to GenericRestReply::onAllErrors
	//! @param failureTransformer Will be passed to GenericRestReply::onAllErrors
	//! @details If the object is already extended it will simply be passed to the handler. If not,
	//! an internal extension is performed, and the handler called for the result.
	//!
	//! The handlers parameters are:
	//! - The deserialized extended object (T*)
	//! - The information, whether the object was extended by sending a request (`true`) or already available (`false`) (bool)
	template<typename ET = QObject*>
	void extend(RestClient *client,
				std::function<void(T*, bool)> extensionHandler,
				std::function<void(QString, int, RestReply::ErrorType)> errorHandler = {},
				std::function<QString(ET, int)> failureTransformer = {});

private:
	QPointer<T> cExt;
};

//! A base class to create a simply type version for a normal one. Q_GADGET version
template<typename T>
class Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>
{
public:
	//! Constructor
	Simple();
	inline virtual ~Simple() = default;

	//! Returns the link to the extended version of this object. @sa #QTRESTCLIENT_EXT_HREF_PROP
	virtual QUrl extensionHref() const = 0;
	//! Returns true, if the object has an extension
	bool hasExtension() const;
	//! Returns true, if the object has already been extended
	bool isExtended() const;
	//! @brief Returns the current extension.
	//! @returns The current extension, a default constructed value, if not extended
	//! @details You will have to extend the object, before you can use this
	T currentExtended() const;

	//! @brief Performs an extension request and returns the reply to the request
	//! @param client The rest client to be used to create the network request
	//! @returns The reply, or `nullptr` if no extension exists
	template<typename ET = QObject*>
	GenericRestReply<T, ET> *extend(RestClient *client);

	//! @brief Extends the object by using a handler
	//! @param client The rest client to be used to create the network request
	//! @param extensionHandler The handler to be called with the extended object
	//! @param errorHandler Will be passed to GenericRestReply::onAllErrors
	//! @param failureTransformer Will be passed to GenericRestReply::onAllErrors
	//! @details If the object is already extended it will simply be passed to the handler. If not,
	//! an internal extension is performed, and the handler called for the result.
	//!
	//! The handlers parameters are:
	//! - The deserialized extended object (T)
	//! - The information, whether the object was extended by sending a request (`true`) or already available (`false`) (bool)
	template<typename ET = QObject*>
	void extend(RestClient *client,
				std::function<void(T, bool)> extensionHandler,
				std::function<void(RestReply*, QString, int, RestReply::ErrorType)> errorHandler = {},
				std::function<QString(ET, int)> failureTransformer = {});

private:
	QSharedPointer<T> cExt;
};

}

//! A simple macro to implement Simple::extensionHref by reading the property with the name x
#define QTRESTCLIENT_EXT_HREF_PROP(x) \
	inline QUrl extensionHref() const override { \
		return property(x).toUrl();\
	}

// ------------- Generic Implementation object -------------

namespace QtRestClient {

template<typename T>
Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>::Simple(QObject *parent) :
	QObject(parent),
	cExt(nullptr)
{}

template<typename T>
bool Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>::hasExtension() const
{
	return extensionHref().isValid();
}

template<typename T>
bool Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>::isExtended() const
{
	return cExt;
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
	if(hasExtension()) {
		return client->rootClass()->get<T*, ET>(extensionHref())
				->onSucceeded([=](int, T *data){
					cExt = data;
				});
	} else
		return nullptr;
}

template<typename T>
template<typename ET>
void Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>::extend(RestClient *client, std::function<void (T *, bool)> extensionHandler, std::function<void (QString, int, RestReply::ErrorType)> errorHandler, std::function<QString (ET, int)> failureTransformer)
{
	if(cExt)
		extensionHandler(cExt, false);
	else if(hasExtension()) {
		client->rootClass()->get<T*, ET>(extensionHref())
				->onSucceeded([=](int, T *data){
					cExt = data;
					extensionHandler(data, true);
				})
				->onAllErrors(errorHandler, failureTransformer);
	}
}

// ------------- Generic Implementation gadget -------------

template<typename T>
Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::Simple() :
	cExt(nullptr)
{}

template<typename T>
bool Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::hasExtension() const
{
	return extensionHref().isValid();
}

template<typename T>
bool Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::isExtended() const
{
	return cExt;
}

template<typename T>
T Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::currentExtended() const
{
	if(cExt)
		return *cExt;
	else
		return T();
}

template<typename T>
template<typename ET>
GenericRestReply<T, ET> *Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::extend(RestClient *client)
{
	if(hasExtension()) {
		return client->rootClass()->get<T, ET>(extensionHref())
				->onSucceeded([=](int, T data){
					cExt.reset(new T(data));
				});
	} else
		return nullptr;
}

template<typename T>
template<typename ET>
void Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::extend(RestClient *client, std::function<void (T, bool)> extensionHandler, std::function<void (RestReply *, QString, int, RestReply::ErrorType)> errorHandler, std::function<QString (ET, int)> failureTransformer)
{
	if(cExt)
		extensionHandler(*cExt, false);
	else if(hasExtension()) {
		client->rootClass()->get<T, ET>(extensionHref())
				->onSucceeded([=](int, T data){
					cExt.reset(new T(data));
					extensionHandler(data, true);
				})
				->onAllErrors(errorHandler, failureTransformer);
	}
}

}

#endif // QTRESTCLIENT_SIMPLEBASE_H
//! @file
