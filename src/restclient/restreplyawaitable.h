#ifndef RESTREPLYAWAITABLE_H
#define RESTREPLYAWAITABLE_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/restreply.h"

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
#include "QtRestClient/genericrestreply.h"
#endif

#if !defined(QT_NO_EXCEPTIONS) && QT_CONFIG(future)
#include <QtCore/QException>
#define Q_RESTCLIENT_EXCEPTION_BASE_CLASS QException
#define Q_RESTCLIENT_EXCEPTION_BASE_OR override
#else
#include <exception>
#define Q_RESTCLIENT_EXCEPTION_BASE_CLASS std::exception
#define Q_RESTCLIENT_EXCEPTION_BASE_OR
#endif

namespace QtRestClient {

//! An exception that is throw on errors when awaiting a RestReply
class Q_RESTCLIENT_EXPORT AwaitedException : public Q_RESTCLIENT_EXCEPTION_BASE_CLASS
{
public:
	//! The exception base class of an AwaitedException. Can either be QException (if defined) or std::exception otherwise
	using Base = Q_RESTCLIENT_EXCEPTION_BASE_CLASS;

	//! Constructor, takes an error code, type and additional data
	AwaitedException(int code, RestReply::ErrorType type, QVariant data);

	//! Returns the error code
	int errorCode() const;
	//! Returns the error type
	RestReply::ErrorType errorType() const;
	//! Returns the additional error data
	QVariant errorData() const;

	//! Converts the error data to json object and returns it
	QJsonObject errorObject() const;
	//! Converts the error data to json array and returns it
	QJsonArray errorArray() const;
	//! Converts the error data to a string and returns it
	QString errorString() const;
	//! Converts the error data to a string using the transformer if neccessary and returns it
	QString errorString(const std::function<QString(QJsonObject, int)> &failureTransformer) const;
	//! @copybrief AwaitedException::errorString(const std::function<QString(QJsonObject, int)> &) const
	QString errorString(const std::function<QString(QJsonArray, int)> &failureTransformer) const;

	//! @inherit{QException::what}
	const char *what() const noexcept override;

	//! @inherit{QException::raise}
	virtual Q_NORETURN void raise() const Q_RESTCLIENT_EXCEPTION_BASE_OR;
	//! @inherit{QException::clone}
	virtual Base *clone() const Q_RESTCLIENT_EXCEPTION_BASE_OR;

protected:
	//! @private
	AwaitedException(const AwaitedException * const other); //MAJOR use copy ctor instead

	//! @private
	const int _code;
	//! @private
	const RestReply::ErrorType _type;
	//! @private
	const QVariant _data;
	//! @private
	mutable QByteArray _msg;
};

class RestReplyAwaitablePrivate;
//! A helper class to be used with [QtCoroutines](https://github.com/Skycoder42/QtCoroutines) to await a rest reply
class Q_RESTCLIENT_EXPORT RestReplyAwaitable
{
public:
	//! Construction form a rest reply
	RestReplyAwaitable(RestReply *reply);
	//! Move constructor
	RestReplyAwaitable(RestReplyAwaitable &&other) noexcept;
	//! Move assignment operator
	RestReplyAwaitable &operator=(RestReplyAwaitable &&other) noexcept;
	~RestReplyAwaitable();

	//! Type returned when awaiting this class
	using type = RestReply::DataType;
	//! Exception type thrown in case of an error
	using exceptionType = AwaitedException;
	//! Prepare the awaitable for resumption
	void prepare(std::function<void()> resume);
	//! Extract the result from the awaitable
	type result();

private:
	QScopedPointer<RestReplyAwaitablePrivate> d;
};



#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
//! An exception that is throw on errors when awaiting a GenericRestReply
template <typename ErrorClassType = QObject*>
class GenericAwaitedException : public AwaitedException
{
public:
	//! Constructor, takes an error code, type and additional data
	GenericAwaitedException(int code, RestReply::ErrorType type, const ErrorClassType &data);
	//! Constructor, takes an error code, type and an error string
	GenericAwaitedException(int code, RestReply::ErrorType type, const QString &data);

	//! @copybrief AwaitedException::errorData
	ErrorClassType genericError() const;
	//! @copybrief AwaitedException::errorString(const std::function<QString(QJsonObject, int)> &) const
	QString errorString(const std::function<QString(ErrorClassType, int)> &failureTransformer) const;

	Q_NORETURN void raise() const override;
	Base *clone() const override;

protected:
	//! @private
	GenericAwaitedException(const GenericAwaitedException * const other);

private:
	QJsonObject errorObject() const;
	QJsonArray errorArray() const;
	QString errorString(const std::function<QString(QJsonObject, int)> &failureTransformer) const;
	QString errorString(const std::function<QString(QJsonArray, int)> &failureTransformer) const;
};

//! A helper class to be used with [QtCoroutines](https://github.com/Skycoder42/QtCoroutines) to await a generic rest reply
template <typename DataClassType, typename ErrorClassType = QObject*>
class GenericRestReplyAwaitable
{
public:
	//! Construction form a generic rest reply
	GenericRestReplyAwaitable(GenericRestReply<DataClassType, ErrorClassType> *reply);
	//! Move constructor
	GenericRestReplyAwaitable(GenericRestReplyAwaitable<DataClassType, ErrorClassType> &&other) noexcept;
	//! Move assignment operator
	GenericRestReplyAwaitable &operator=(GenericRestReplyAwaitable<DataClassType, ErrorClassType> &&other) noexcept;

	//! Type returned when awaiting this class
	using type = DataClassType;
	//! Exception type thrown in case of an error
	using exceptionType = GenericAwaitedException<ErrorClassType>;
	//! Prepare the awaitable for resumption
	void prepare(const std::function<void()> &resume);
	//! Extract the result from the awaitable
	type result();

private:
	QPointer<GenericRestReply<DataClassType, ErrorClassType>> reply;
	DataClassType successResult;
	QScopedPointer<exceptionType> errorResult;
};

//! @copybrief QtRestClient::GenericRestReplyAwaitable
template <typename ErrorClassType>
class GenericRestReplyAwaitable<void, ErrorClassType>
{
public:
	//! @copybrief GenericRestReplyAwaitable::GenericRestReplyAwaitable(GenericRestReply<DataClassType, ErrorClassType> *)
	GenericRestReplyAwaitable(GenericRestReply<void, ErrorClassType> *reply);
	//! @copybrief GenericRestReplyAwaitable::GenericRestReplyAwaitable(GenericRestReplyAwaitable<DataClassType, ErrorClassType> &&)
	GenericRestReplyAwaitable(GenericRestReplyAwaitable<void, ErrorClassType> &&other) noexcept;
	//! @copybrief GenericRestReplyAwaitable::operator=(GenericRestReplyAwaitable<DataClassType, ErrorClassType> &&)
	GenericRestReplyAwaitable &operator=(GenericRestReplyAwaitable<void, ErrorClassType> &&other) noexcept ;

	//! @copybrief GenericRestReplyAwaitable::type
	using type = void;
	//! @copybrief GenericRestReplyAwaitable::exceptionType
	using exceptionType = GenericAwaitedException<ErrorClassType>;
	//! @copybrief GenericRestReplyAwaitable::prepare
	void prepare(const std::function<void()> &resume);
	//! @copybrief GenericRestReplyAwaitable::result
	type result();

private:
	QPointer<GenericRestReply<void, ErrorClassType>> reply;
	QScopedPointer<exceptionType> errorResult;
};
#endif

// ------------- Generic Implementation -------------

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
template<typename DataClassType, typename ErrorClassType>
GenericRestReplyAwaitable<DataClassType, ErrorClassType>::GenericRestReplyAwaitable(GenericRestReply<DataClassType, ErrorClassType> *reply) :
	reply{reply}
{}

template<typename DataClassType, typename ErrorClassType>
GenericRestReplyAwaitable<DataClassType, ErrorClassType>::GenericRestReplyAwaitable(GenericRestReplyAwaitable<DataClassType, ErrorClassType> &&other) noexcept :
	reply{other.reply},
	successResult{std::move(other.successResult)}
{
	errorResult.swap(other.errorResult);
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReplyAwaitable<DataClassType, ErrorClassType> &GenericRestReplyAwaitable<DataClassType, ErrorClassType>::operator=(GenericRestReplyAwaitable<DataClassType, ErrorClassType> &&other) noexcept
{
	reply = other.reply;
	successResult = std::move(other.successResult);
	errorResult.swap(other.errorResult);
	return *this;
}

template<typename DataClassType, typename ErrorClassType>
void GenericRestReplyAwaitable<DataClassType, ErrorClassType>::prepare(const std::function<void ()> &resume)
{
	reply->onSucceeded([this, resume](int, DataClassType data) {
		errorResult.reset();
		successResult = std::move(data);
		resume();
	});
	reply->onFailed([this, resume](int code, ErrorClassType data) {
		errorResult.reset(new exceptionType{code, RestReply::FailureError, std::move(data)});
		resume();
	});
	reply->onSerializeException([this, resume](const QtJsonSerializer::Exception &data) {
		errorResult.reset(new exceptionType{0, RestReply::DeserializationError, QString::fromUtf8(data.what())});
		resume();
	});
	reply->onError([this, resume](QString message, int code, RestReply::ErrorType type) {
		errorResult.reset(new exceptionType{code, type, std::move(message)});
		resume();
	});
}

template<typename DataClassType, typename ErrorClassType>
typename GenericRestReplyAwaitable<DataClassType, ErrorClassType>::type GenericRestReplyAwaitable<DataClassType, ErrorClassType>::result()
{
	if(errorResult) {
		errorResult->raise();
		Q_UNREACHABLE();
	} else
		return successResult;
}



template<typename ErrorClassType>
GenericRestReplyAwaitable<void, ErrorClassType>::GenericRestReplyAwaitable(GenericRestReply<void, ErrorClassType> *reply) :
	reply{reply}
{}

template<typename ErrorClassType>
GenericRestReplyAwaitable<void, ErrorClassType>::GenericRestReplyAwaitable(GenericRestReplyAwaitable<void, ErrorClassType> &&other) noexcept :
	reply{other.reply}
{
	errorResult.swap(other.errorResult);
}

template<typename ErrorClassType>
GenericRestReplyAwaitable<void, ErrorClassType> &GenericRestReplyAwaitable<void, ErrorClassType>::operator=(GenericRestReplyAwaitable<void, ErrorClassType> &&other) noexcept
{
	reply = other.reply;
	errorResult.swap(other.errorResult);
	return *this;
}

template<typename ErrorClassType>
void GenericRestReplyAwaitable<void, ErrorClassType>::prepare(const std::function<void ()> &resume)
{
	reply->onSucceeded([this, resume](int) {
		errorResult.reset();
		resume();
	});
	reply->onFailed([this, resume](int code, ErrorClassType data) {
		errorResult.reset(new exceptionType{code, RestReply::FailureError, std::move(data)});
		resume();
	});
	reply->onSerializeException([this, resume](const QtJsonSerializer::Exception &data) {
		errorResult.reset(new exceptionType{0, RestReply::DeserializationError, QString::fromUtf8(data.what())});
		resume();
	});
	reply->onError([this, resume](QString message, int code, RestReply::ErrorType type) {
		errorResult.reset(new exceptionType{code, type, std::move(message)});
		resume();
	});
}

template<typename ErrorClassType>
typename GenericRestReplyAwaitable<void, ErrorClassType>::type GenericRestReplyAwaitable<void, ErrorClassType>::result()
{
	if(errorResult) {
		errorResult->raise();
		Q_UNREACHABLE();
	}
}



template<typename ErrorClassType>
GenericAwaitedException<ErrorClassType>::GenericAwaitedException(int code, RestReply::ErrorType type, const ErrorClassType &data) :
	AwaitedException{code, type, QVariant::fromValue<ErrorClassType>(data)}
{}

template<typename ErrorClassType>
GenericAwaitedException<ErrorClassType>::GenericAwaitedException(int code, RestReply::ErrorType type, const QString &data) :
	AwaitedException{code, type, data}
{}

template<typename ErrorClassType>
GenericAwaitedException<ErrorClassType>::GenericAwaitedException(const GenericAwaitedException * const other) :
	AwaitedException{other}
{}

template<typename ErrorClassType>
ErrorClassType GenericAwaitedException<ErrorClassType>::genericError() const
{
	return _data.template value<ErrorClassType>();
}

template<typename ErrorClassType>
QString GenericAwaitedException<ErrorClassType>::errorString(const std::function<QString (ErrorClassType, int)> &failureTransformer) const
{
	if(_type == RestReply::FailureError)
		return failureTransformer(genericError(), _code);
	else
		return errorString();
}

template<typename ErrorClassType>
void GenericAwaitedException<ErrorClassType>::raise() const
{
	throw *this;
}

template<typename ErrorClassType>
AwaitedException::Base *GenericAwaitedException<ErrorClassType>::clone() const
{
	return new GenericAwaitedException<ErrorClassType>{this};
}

template<typename ErrorClassType>
QJsonObject GenericAwaitedException<ErrorClassType>::errorObject() const
{
	return AwaitedException::errorObject();
}

template<typename ErrorClassType>
QJsonArray GenericAwaitedException<ErrorClassType>::errorArray() const
{
	return AwaitedException::errorArray();
}

template<typename ErrorClassType>
QString GenericAwaitedException<ErrorClassType>::errorString(const std::function<QString (QJsonObject, int)> &failureTransformer) const
{
	return AwaitedException::errorString(failureTransformer);
}

template<typename ErrorClassType>
QString GenericAwaitedException<ErrorClassType>::errorString(const std::function<QString (QJsonArray, int)> &failureTransformer) const
{
	return AwaitedException::errorString(failureTransformer);
}

// await method implementations

template<typename TInstance, typename DataClassType, typename ErrorClassType>
GenericRestReplyAwaitable<DataClassType, ErrorClassType> GenericRestReplyBase<TInstance, DataClassType, ErrorClassType>::awaitable()
{
	return GenericRestReplyAwaitable<DataClassType, ErrorClassType>{this};
}
#endif

}

#endif // RESTREPLYAWAITABLE_H
