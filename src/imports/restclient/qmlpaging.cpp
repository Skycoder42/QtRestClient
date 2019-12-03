#include "qmlpaging.h"
#include <QDebug>
#include <QPointer>
using namespace QtRestClient;

QmlPaging::QmlPaging(IPaging *iPaging, RestClient *client, QJSEngine *engine) :
	_engine(engine),
	_client(client),
	_paging(iPaging)
{}

QmlPaging QmlPaging::create(RestClient *client, QJSEngine *engine, const QJsonObject &obj)
{
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	auto iPaging = client->pagingFactory()->createPaging(client->serializer(), obj);
#else
	auto iPaging = client->pagingFactory()->createPaging(obj);
#endif
	return QmlPaging{iPaging, client, engine};
}

RestReply *QmlPaging::next()
{
	if (_paging->hasNext()) {
		auto reply = _client->builder()
				.updateFromRelativeUrl(_paging->next(), true)
				.send();
		return new RestReply{reply, _client};
	} else
		return nullptr;
}

RestReply *QmlPaging::previous()
{
	if (_paging->hasPrevious()) {
		auto reply = _client->builder()
				.updateFromRelativeUrl(_paging->previous(), true)
				.send();
		return new RestReply{reply, _client};
	} else
		return nullptr;
}

QVariantList QmlPaging::items() const
{
	return std::visit([](const auto &items){
		return items.toVariantList();
	}, _paging->items());
}

int QmlPaging::total() const
{
	return static_cast<int>(_paging->total());
}

int QmlPaging::offset() const
{
	return static_cast<int>(_paging->offset());
}

bool QmlPaging::hasNext() const
{
	return _paging->hasNext();
}

QUrl QmlPaging::nextUrl() const
{
	return _paging->next();
}

bool QmlPaging::hasPrevious() const
{
	return _paging->hasPrevious();
}

QUrl QmlPaging::previousUrl() const
{
	return _paging->previous();
}

QVariantMap QmlPaging::properties() const
{
	return _paging->properties();
}

void QmlPaging::iterate(const QJSValue &iterator, int to, int from)
{
	iterate(iterator, {}, {}, to, from);
}

void QmlPaging::iterate(const QJSValue &iterator, const QJSValue &failureHandler, const QJSValue &errorHandler, int to, int from)
{
	// TODO check if still ok
	if (!iterator.isCallable()) {
		qWarning() << "iterator parameter must be a function";
		return;
	}
	if (!failureHandler.isUndefined() && !failureHandler.isCallable()) {
		qWarning() << "failureHandler parameter must be a function or undefined";
		return;
	}
	if (!errorHandler.isUndefined() && !errorHandler.isCallable()) {
		qWarning() << "errorHandler parameter must be a function or undefined";
		return;
	}
	if (from < _paging->offset()) {
		qWarning() << "from must be smaller then offset" << from << _paging->offset();
		return;
	}

	auto index = internalIterate(iterator, from, to);
	if(index < 0)
		return;

	// calc total limit -> only if supports indexes
	int max = INT_MAX;
	if (_paging->offset() >= 0) {
		if(to >= 0)
			max = std::min(to, static_cast<int>(_paging->total()));
		else
			max = static_cast<int>(_paging->total());
	}

	// continue to the next one
	if (index < max && _paging->hasNext()) {
		QPointer<RestClient> client{_client};
		QPointer<QJSEngine> engine{_engine};

		auto reply = next()->onSucceeded([client, engine, iterator, failureHandler, errorHandler, to, index](int, const QJsonObject &data) {
			if (!client || !engine)
				return;
			auto paging = create(client, engine, data);
			paging.iterate(iterator, failureHandler, errorHandler, to, index);
		});

		if (failureHandler.isCallable()) {
			reply->onFailed([engine, failureHandler](int code, const QJsonObject &data) {
				if (!engine)
					return;
				auto fn = failureHandler;
				fn.call({
							code,
							engine->toScriptValue(data)
						});
			});
		}
		if (errorHandler.isCallable()) {
			reply->onError([errorHandler](const QString &error, int code, RestReply::Error type) {
				auto fn = errorHandler;
				fn.call({error, code, static_cast<int>(type)});
			});
		}
	}
}

int QmlPaging::internalIterate(QJSValue iterator, int from, int to) const
{
	// TODO check if still ok
	return std::visit([&](const auto &items) {
		// handle all items in this paging
		auto offset = _paging->offset();
		auto count = static_cast<qint64>(items.size());
		auto start = 0ll;
		auto max = count;
		if (offset >= 0) {// has indexes
			// from
			start = std::max(static_cast<qint64>(from), offset) - offset;
			// to
			if(to >= 0)
				max = std::min(static_cast<qint64>(to), max + offset) - offset;
		}

		// iterate over used items
		qint64 i;
		auto canceled = false;
		for (i = start; i < max; i++) {
			auto item = items[i].toVariant();
			auto index = offset >= 0 ? i + offset : -1;
			auto res = iterator.call({
				_engine->toScriptValue(item),
				static_cast<int>(index)
			});
			if (!res.toBool()) {
				canceled = true;
				break;
			}
		}

		if (canceled)
			return -1;
		else if (offset >= 0)
			return static_cast<int>(offset + i);
		else
			return 0;
	}, _paging->items());
}
