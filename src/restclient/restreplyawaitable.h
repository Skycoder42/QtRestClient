#ifndef RESTREPLYAWAITABLE_H
#define RESTREPLYAWAITABLE_H

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/restreply.h"
#include "QtRestClient/genericrestreply.h"

#include <QtCore/qexception.h>

namespace QtRestClient {

class AwaitedException : public QException
{
public:
	AwaitedException(int code, RestReply::ErrorType type, QVariant data);

	int errorCode() const;
	RestReply::ErrorType errorType() const;
	QVariant errorData() const;

	QJsonObject errorObject() const;
	QJsonArray errorArray() const;
	QString errorString() const;
	QString errorString(const std::function<QString(QJsonObject, int)> &failureTransformer) const;
	QString errorString(const std::function<QString(QJsonArray, int)> &failureTransformer) const;

	const char *what() const noexcept override;

	Q_NORETURN void raise() const override;
	QException *clone() const override;

protected:
	AwaitedException(const AwaitedException * const other);

	const int _code;
	const RestReply::ErrorType _type;
	const QVariant _data;
	mutable QByteArray _msg;
};

class RestReplyAwaitablePrivate;
class Q_RESTCLIENT_EXPORT RestReplyAwaitable
{
public:
	RestReplyAwaitable(RestReply *reply);
	RestReplyAwaitable(RestReplyAwaitable &&other) noexcept;
	RestReplyAwaitable &operator=(RestReplyAwaitable &&other) noexcept;
	~RestReplyAwaitable();

	//! @private
	using type = QJsonValue;
	//! @private
	using exceptionType = AwaitedException;
	//! @private
	void prepare(std::function<void()> resume);
	//! @private
	type result();

private:
	QScopedPointer<RestReplyAwaitablePrivate> d;
};



template <typename ErrorClassType = QObject*>
class GenericAwaitedException : public AwaitedException
{
public:
	GenericAwaitedException(int code, RestReply::ErrorType type, ErrorClassType data);
	GenericAwaitedException(int code, RestReply::ErrorType type, const QString &data);

	ErrorClassType genericError() const;
	QString errorString(const std::function<QString(ErrorClassType, int)> &failureTransformer) const;

	Q_NORETURN void raise() const override;
	QException *clone() const override;

protected:
	GenericAwaitedException(const GenericAwaitedException * const other);

private:
	QJsonObject errorObject() const;
	QJsonArray errorArray() const;
	QString errorString(const std::function<QString(QJsonObject, int)> &failureTransformer) const;
	QString errorString(const std::function<QString(QJsonArray, int)> &failureTransformer) const;
};

template <typename DataClassType, typename ErrorClassType = QObject*>
class GenericRestReplyAwaitable
{
public:
	GenericRestReplyAwaitable(GenericRestReply<DataClassType, ErrorClassType> *reply);
	GenericRestReplyAwaitable(GenericRestReplyAwaitable<DataClassType, ErrorClassType> &&other) noexcept = default;
	GenericRestReplyAwaitable &operator=(GenericRestReplyAwaitable<DataClassType, ErrorClassType> &&other) noexcept = default;

	//! @private
	using type = DataClassType;
	using exceptionType = GenericAwaitedException<ErrorClassType>;
	//! @private
	void prepare(std::function<void()> resume);
	//! @private
	type result(); //TODO doc throws json value

private:
	QPointer<GenericRestReply<DataClassType, ErrorClassType>> reply;
	DataClassType successResult;
	QScopedPointer<exceptionType> errorResult;
};

template <typename ErrorClassType>
class GenericRestReplyAwaitable<void, ErrorClassType>
{
public:
	GenericRestReplyAwaitable(GenericRestReply<void, ErrorClassType> *reply);
	GenericRestReplyAwaitable(GenericRestReplyAwaitable<void, ErrorClassType> &&other) noexcept = default;
	GenericRestReplyAwaitable &operator=(GenericRestReplyAwaitable<void, ErrorClassType> &&other) noexcept = default;

	//! @private
	using type = void;
	using exceptionType = GenericAwaitedException<ErrorClassType>;
	//! @private
	void prepare(std::function<void()> resume);
	//! @private
	type result(); //TODO doc throws json value

private:
	QPointer<GenericRestReply<void, ErrorClassType>> reply;
	QScopedPointer<exceptionType> errorResult;
};

// ------------- Generic Implementation -------------

template<typename DataClassType, typename ErrorClassType>
GenericRestReplyAwaitable<DataClassType, ErrorClassType>::GenericRestReplyAwaitable(GenericRestReply<DataClassType, ErrorClassType> *reply) :
	reply{reply}
{}

template<typename DataClassType, typename ErrorClassType>
void GenericRestReplyAwaitable<DataClassType, ErrorClassType>::prepare(std::function<void ()> resume)
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
	reply->onSerializeException([this, resume](const QJsonSerializerException &data) {
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
void GenericRestReplyAwaitable<void, ErrorClassType>::prepare(std::function<void ()> resume)
{
	reply->onSucceeded([this, resume](int) {
		errorResult.reset();
		resume();
	});
	reply->onFailed([this, resume](int code, ErrorClassType data) {
		errorResult.reset(new exceptionType{code, RestReply::FailureError, std::move(data)});
		resume();
	});
	reply->onSerializeException([this, resume](const QJsonSerializerException &data) {
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

// await method implementations

template<typename DataClassType, typename ErrorClassType>
GenericRestReplyAwaitable<DataClassType, ErrorClassType> GenericRestReply<DataClassType, ErrorClassType>::awaitable()
{
	return GenericRestReplyAwaitable<DataClassType, ErrorClassType>{this};
}

template<typename ErrorClassType>
GenericRestReplyAwaitable<void, ErrorClassType> GenericRestReply<void, ErrorClassType>::awaitable()
{
	return GenericRestReplyAwaitable<void, ErrorClassType>{this};
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReplyAwaitable<QList<DataClassType>, ErrorClassType> GenericRestReply<QList<DataClassType>, ErrorClassType>::awaitable()
{
	return GenericRestReplyAwaitable<QList<DataClassType>, ErrorClassType>{this};
}

template<typename DataClassType, typename ErrorClassType>
GenericRestReplyAwaitable<Paging<DataClassType>, ErrorClassType> GenericRestReply<Paging<DataClassType>, ErrorClassType>::awaitable()
{
	return GenericRestReplyAwaitable<Paging<DataClassType>, ErrorClassType>{this};
}

}

#endif // RESTREPLYAWAITABLE_H
