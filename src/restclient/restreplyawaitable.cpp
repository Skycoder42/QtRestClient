#include "restreplyawaitable.h"
#include "restreplyawaitable_p.h"

#include <QtCore/QMetaEnum>
using namespace QtRestClient;

RestReplyAwaitable::RestReplyAwaitable(RestReply *reply) :
	d{new RestReplyAwaitablePrivate{}}
{
	d->reply = reply;
}

RestReplyAwaitable::RestReplyAwaitable(RestReplyAwaitable &&other) noexcept
{
	d.swap(other.d);
}

RestReplyAwaitable &RestReplyAwaitable::operator=(RestReplyAwaitable &&other) noexcept
{
	d.swap(other.d);
	return *this;
}

RestReplyAwaitable::~RestReplyAwaitable() = default;

void RestReplyAwaitable::prepare(const std::function<void()> &resume)
{
	d->reply->onSucceeded(d->reply, [this, resume](RestReply::DataType data){
		d->errorResult.reset();
		d->successResult = std::move(data);
		resume();
	});
	d->reply->onFailed(d->reply, [this, resume](int code, const RestReply::DataType &data){
		d->errorResult.reset(new AwaitedException {
			code,
			RestReply::Error::Failure,
			std::visit(__private::overload {
						   [](std::nullopt_t) {
							   return QVariant{};
						   },
						   [](auto vData) {
							   return vData.toVariant();
						   }
					   }, data)});
		resume();
	});
	d->reply->onError([this, resume](const QString &errorString, int code, RestReply::Error type) {
		d->errorResult.reset(new AwaitedException{code, type, errorString});
		resume();
	});
}

RestReplyAwaitable::type RestReplyAwaitable::result()
{
	if(d->errorResult) {
		d->errorResult->raise();
		Q_UNREACHABLE();
	} else
		return d->successResult;
}



AwaitedException::AwaitedException(int code, RestReply::Error type, QVariant data) :
	_code{code},
	_type{type},
	_data{std::move(data)}
{}

int AwaitedException::errorCode() const
{
	return _code;
}

RestReply::Error AwaitedException::errorType() const
{
	return _type;
}

QVariant AwaitedException::errorData() const
{
	return _data;
}

QVariantMap AwaitedException::errorObject() const
{
	return _data.toMap();
}

QVariantList AwaitedException::errorArray() const
{
	return _data.toList();
}

QString AwaitedException::errorString() const
{
	return _data.toString();
}

QString AwaitedException::errorString(const std::function<QString (QVariantMap, int)> &failureTransformer) const
{
	if(_type == RestReply::Error::Failure)
		return failureTransformer(errorObject(), _code);
	else
		return errorString();
}

QString AwaitedException::errorString(const std::function<QString (QVariantList, int)> &failureTransformer) const
{
	if(_type == RestReply::Error::Failure)
		return failureTransformer(errorArray(), _code);
	else
		return errorString();
}

const char *AwaitedException::what() const noexcept
{
	if (_msg.isEmpty()) {
		auto mEnum = QMetaEnum::fromType<RestReply::Error>();
		_msg = QByteArray(mEnum.valueToKey(static_cast<int>(_type))) + ": " +
			   QByteArray::number(_code) + ", " +
			   _data.toString().toUtf8();
	}
	return _msg.constData();
}

void AwaitedException::raise() const
{
	throw *this;
}

ExceptionBase *AwaitedException::clone() const
{
	return new AwaitedException{*this};
}
