#ifndef QTRESTCLIENT_SIMPLEBASE_H
#define QTRESTCLIENT_SIMPLEBASE_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/genericrestreply.h"
#include "QtRestClient/restclass.h"

#include <QtCore/qpointer.h>
#include <QtCore/qsharedpointer.h>
#include <QtCore/qscopedpointer.h>
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
				const std::function<void(T*, bool)> &extensionHandler,
				const std::function<void(QString, int, RestReply::ErrorType)> &errorHandler = {},
				const std::function<QString(ET, int)> &failureTransformer = {});

private:
	QSharedPointer<QPointer<T>> _ext;
	inline QPointer<T> &ext();
	inline const QPointer<T> &ext() const;
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
				const std::function<void(T, bool)> &extensionHandler,
				const std::function<void(QString, int, RestReply::ErrorType)> &errorHandler = {},
				const std::function<QString(ET, int)> &failureTransformer = {});

private:
	QSharedPointer<QScopedPointer<T>> _ext;
	inline QScopedPointer<T> &ext();
	inline const QScopedPointer<T> &ext() const;
};

}

//! A simple macro to implement Simple::extensionHref by reading the property with the name x
#define QTRESTCLIENT_EXT_HREF_PROP(x) \
	inline QUrl extensionHref() const override { \
		return property(#x).toUrl();\
	}

// ------------- Generic Implementation object -------------

namespace QtRestClient {

template<typename T>
Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>::Simple(QObject *parent) :
	QObject(parent),
	_ext(new QPointer<T>{})
{}

template<typename T>
bool Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>::hasExtension() const
{
	return extensionHref().isValid();
}

template<typename T>
bool Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>::isExtended() const
{
	return ext();
}

template<typename T>
T *Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>::currentExtended() const
{
	return ext();
}

template<typename T>
template<typename ET>
GenericRestReply<T*, ET> *Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>::extend(RestClient *client)
{
	if(hasExtension()) {
		QWeakPointer<QPointer<T>> extRef = _ext;
		return client->rootClass()->get<T*, ET>(extensionHref())
				->onSucceeded([extRef](int, T *data) {
					if(auto ext = extRef.toStrongRef())
						*ext = data;
				});
	} else
		return nullptr;
}

template<typename T>
template<typename ET>
void Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>::extend(RestClient *client, const std::function<void (T *, bool)> &extensionHandler, const std::function<void (QString, int, RestReply::ErrorType)> &errorHandler, const std::function<QString (ET, int)> &failureTransformer)
{
	if(ext())
		extensionHandler(ext(), false);
	else if(hasExtension()) {
		QWeakPointer<QPointer<T>> extRef = _ext;
		client->rootClass()->get<T*, ET>(extensionHref())
				->onSucceeded([extRef, extensionHandler](int, T *data){
					if(auto ext = extRef.toStrongRef())
						*ext = data;
					extensionHandler(data, true);
				})
				->onAllErrors(errorHandler, failureTransformer);
	}
}

template<typename T>
QPointer<T> &QtRestClient::Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>::ext()
{
	return *_ext;
}

template<typename T>
const QPointer<T> &QtRestClient::Simple<T*, typename std::enable_if<std::is_base_of<QObject, T>::value>::type>::ext() const
{
	return *_ext;
}

// ------------- Generic Implementation gadget -------------

template<typename T>
Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::Simple() :
	_ext(new QScopedPointer<T>{nullptr})
{}

template<typename T>
bool Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::hasExtension() const
{
	return extensionHref().isValid();
}

template<typename T>
bool Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::isExtended() const
{
	return ext();
}

template<typename T>
T Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::currentExtended() const
{
	if(ext())
		return *(ext().data());
	else
		return T();
}

template<typename T>
template<typename ET>
GenericRestReply<T, ET> *Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::extend(RestClient *client)
{
	if(hasExtension()) {
		QWeakPointer<QScopedPointer<T>> extRef = _ext;
		return client->rootClass()->get<T, ET>(extensionHref())
				->onSucceeded([extRef](int, T data){
					if(auto ext = extRef.toStrongRef())
						ext.data()->reset(new T(data));
				});
	} else
		return nullptr;
}

template<typename T>
template<typename ET>
void Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::extend(RestClient *client, const std::function<void (T, bool)> &extensionHandler, const std::function<void (QString, int, RestReply::ErrorType)> &errorHandler, const std::function<QString (ET, int)> &failureTransformer)
{
	if(ext())
		extensionHandler(*(ext().data()), false);
	else if(hasExtension()) {
		QWeakPointer<QScopedPointer<T>> extRef = _ext;
		client->rootClass()->get<T, ET>(extensionHref())
				->onSucceeded([extRef, extensionHandler](int, T data){
					if(auto ext = extRef.toStrongRef())
						ext.data()->reset(new T(data));
					extensionHandler(data, true);
				})
				->onAllErrors(errorHandler, failureTransformer);
	}
}

template<typename T>
QScopedPointer<T> &QtRestClient::Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::ext()
{
	return *_ext;
}

template<typename T>
const QScopedPointer<T> &QtRestClient::Simple<T, typename std::enable_if<std::is_void<typename T::QtGadgetHelper>::value>::type>::ext() const
{
	return *_ext;
}

}

#endif // QTRESTCLIENT_SIMPLEBASE_H
//! @file
