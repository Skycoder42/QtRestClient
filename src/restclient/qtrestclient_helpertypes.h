#ifndef QTRESTCLIENT_HELPERTYPES_H
#define QTRESTCLIENT_HELPERTYPES_H

#include "QtRestClient/qtrestclient_global.h"

#include <variant>

#include <QtCore/qcborvalue.h>
#include <QtCore/qcbormap.h>
#include <QtCore/qcborarray.h>
#include <QtCore/qjsonvalue.h>
#include <QtCore/qjsonobject.h>
#include <QtCore/qjsonarray.h>

namespace QtRestClient::__private {

template<typename... Ts>
struct overload : public Ts... {
	using Ts::operator()...;
};
template<class... Ts>
overload(Ts...) -> overload<Ts...>;

namespace __binder {

using DataType = std::variant<std::nullopt_t, QCborValue, QJsonValue>;
using FunctionType = std::function<void(int, DataType)>;

template <typename... TArgs>
struct FnBinder;

template <>
struct FnBinder<int, DataType> {
	template <typename TFn>
	static inline FunctionType bind(TFn &&fn) {
		return std::forward<TFn>(fn);
	}
};

template <>
struct FnBinder<> {
	template <typename TFn>
	static inline FunctionType bind(TFn &&fn) {
		return [xFn = std::forward<TFn>(fn)](int, const DataType &) {
			xFn();
		};
	}
};

template <>
struct FnBinder<int> {
	template <typename TFn>
	static inline FunctionType bind(TFn &&fn) {
		return [xFn = std::forward<TFn>(fn)](int code, const DataType &) {
			xFn(code);
		};
	}
};

template <typename TData>
struct FnBinder<TData> {
	template <typename TFn>
	static inline FunctionType bind(TFn &&fn) {
		return FnBinder<int, TData>::bind([xFn = std::forward<TFn>(fn)](int, const TData &value) {  // TData is already decayed -> true move
			xFn(value);
		});
	}
};

template <>
struct FnBinder<int, QJsonValue> {
	template <typename TFn>
	static inline FunctionType bind(TFn &&fn) {
		return [xFn = std::forward<TFn>(fn)](int code, const DataType &value) {
			std::visit(overload {
						   [xFn, code](std::nullopt_t){
							   xFn(code, QJsonValue{QJsonValue::Undefined});
						   },
						   [xFn, code](const QCborValue &){
							   qCWarning(logGlobal) << "CBOR data in JSON callback - discarding";
						   },
						   [xFn, code](const QJsonValue &vValue){
							   xFn(code, vValue);
						   }
					   }, value);
		};
	}
};

template <>
struct FnBinder<int, QJsonObject> {
	template <typename TFn>
	static inline FunctionType bind(TFn &&fn) {
		return [xFn = std::forward<TFn>(fn)](int code, const DataType &value) {
			std::visit(overload {
						   [xFn, code](std::nullopt_t){
							   xFn(code, QJsonObject{});
						   },
						   [xFn, code](const QCborValue &){
							   qCWarning(logGlobal) << "CBOR data in JSON callback - discarding";
						   },
						   [xFn, code](const QJsonValue &vValue){
							   xFn(code, vValue.toObject());
						   }
					   }, value);
		};
	}
};

template <>
struct FnBinder<int, QJsonArray> {
	template <typename TFn>
	static inline FunctionType bind(TFn &&fn) {
		return [xFn = std::forward<TFn>(fn)](int code, const DataType &value) {
			std::visit(overload {
						   [xFn, code](std::nullopt_t){
							   xFn(code, QJsonArray{});
						   },
						   [xFn, code](const QCborValue &){
							   qCWarning(logGlobal) << "CBOR data in JSON callback - discarding";
						   },
						   [xFn, code](const QJsonValue &vValue){
							   xFn(code, vValue.toArray());
						   }
					   }, value);
		};
	}
};

template <>
struct FnBinder<int, QCborValue> {
	template <typename TFn>
	static inline FunctionType bind(TFn &&fn) {
		return [xFn = std::forward<TFn>(fn)](int code, const DataType &value) {
			std::visit(overload {
						   [xFn, code](std::nullopt_t){
							   xFn(code, QCborValue{QCborSimpleType::Undefined});
						   },
						   [xFn, code](const QCborValue &vValue){
							   xFn(code, vValue);
						   },
						   [xFn, code](const QJsonValue &){
							   qCWarning(logGlobal) << "JSON data in CBOR callback - discarding";
						   }
					   }, value);
		};
	}
};

template <>
struct FnBinder<int, QCborMap> {
	template <typename TFn>
	static inline FunctionType bind(TFn &&fn) {
		return [xFn = std::forward<TFn>(fn)](int code, const DataType &value) {
			std::visit(overload {
						   [xFn, code](std::nullopt_t){
							   xFn(code, QCborMap{});
						   },
						   [xFn, code](const QCborValue &vValue){
							   xFn(code, vValue.toMap());
						   },
						   [xFn, code](const QJsonValue &){
							   qCWarning(logGlobal) << "JSON data in CBOR callback - discarding";
						   }
					   }, value);
		};
	}
};

template <>
struct FnBinder<int, QCborArray> {
	template <typename TFn>
	static inline FunctionType bind(TFn &&fn) {
		return [xFn = std::forward<TFn>(fn)](int code, const DataType &value) {
			std::visit(overload {
						   [xFn, code](std::nullopt_t){
							   xFn(code, QCborArray{});
						   },
						   [xFn, code](const QCborValue &vValue){
							   xFn(code, vValue.toArray());
						   },
						   [xFn, code](const QJsonValue &){
							   qCWarning(logGlobal) << "JSON data in CBOR callback - discarding";
						   }
					   }, value);
		};
	}
};

template <typename TCbor, typename TJson>
struct FnBinder<int, std::variant<std::nullopt_t, TCbor, TJson>> {
	template <typename TFn>
	static inline FunctionType bind(TFn &&fn) {
		return [xFn = std::forward<TFn>(fn)](int code, const DataType &value) {
			auto cFn = FnBinder<int, TCbor>::bind(xFn)(code, value);
			auto jFn = FnBinder<int, TJson>::bind(xFn)(code, value);
			std::visit(overload {
						   [xFn, code](std::nullopt_t){
							   xFn(code, std::nullopt);
						   },
						   [xcFn = std::move(cFn), code](const QCborValue &vValue){
							   xcFn(code, vValue);
						   },
						   [xjFn = std::move(jFn), code](const QJsonValue &vValue){
							   xjFn(code, vValue);
						   }
					   }, value);
		};
	}
};

template <typename TCbor, typename TJson>
struct FnBinder<int, std::variant<TCbor, TJson>> {
	template <typename TFn>
	static inline FunctionType bind(TFn &&fn) {
		return [xFn = std::forward<TFn>(fn)](int code, const DataType &value) {
			auto cFn = FnBinder<int, TCbor>::bind(xFn)(code, value);
			auto jFn = FnBinder<int, TJson>::bind(xFn)(code, value);
			std::visit(overload {
						   [xFn, code](std::nullopt_t){
							   xFn(code, std::variant<TCbor, TJson>{});
						   },
						   [xcFn = std::move(cFn), code](const QCborValue &vValue){
							   xcFn(code, vValue);
						   },
						   [xjFn = std::move(jFn), code](const QJsonValue &vValue){
							   xjFn(code, vValue);
						   }
					   }, value);
		};
	}
};


template <typename T>
struct unforce {};

template <template<class> class C, typename... TArgs>
struct unforce<C<TArgs...>> {
	template <typename... TImpl>
	using type = C<TImpl...>;
};

void stdBindEvalFn(int);

template <typename... TArgs>
using stdBind = typename unforce<decltype(std::bind(stdBindEvalFn, 0))>::type<TArgs...>;

template <typename T>
struct FnInfo : public FnInfo<decltype(&T::operator())> {};

template <typename T, typename... TArgs>
struct FnInfo<stdBind<T, TArgs...>> : public FnInfo<T> {};

template <typename TClass, typename TRet, typename... TArgs>
struct FnInfo<TRet(TClass::*)(TArgs...) const>
{
	using Binder = FnBinder<std::decay_t<TArgs>...>;
};

template <typename TClass, typename TRet, typename... TArgs>
struct FnInfo<TRet(TClass::*)(TArgs...)>
{
	using Binder = FnBinder<std::decay_t<TArgs>...>;
};

template <typename TRet, typename... TArgs>
struct FnInfo<TRet(*)(TArgs...)>
{
	using Binder = FnBinder<std::decay_t<TArgs>...>;
};

template <typename TClass, typename TRet, typename... TBinders, typename... TArgs>
struct FnInfo<TRet(TClass::*(TBinders...))(TArgs...) const>
{
	using Binder = FnBinder<std::decay_t<TArgs>...>;
};

template <typename TClass, typename TRet, typename... TBinders, typename... TArgs>
struct FnInfo<TRet(TClass::*(TBinders...))(TArgs...)>
{
	using Binder = FnBinder<std::decay_t<TArgs>...>;
};

}

template <typename TFn>
static inline __binder::FunctionType bindCallback(TFn &&fn) {
	return __binder::FnInfo<std::decay_t<TFn>>::Binder::bind(std::forward<TFn>(fn));
}

template <typename TFn, typename TError>
static inline __binder::FunctionType bindCallback(const std::function<void(QString, int, TError)> &handler, TFn &&tFn, TError errorType) {
	return bindCallback([handler, xTFn = std::forward<TFn>(tFn), errorType](int code, auto &&data) {
		handler(xTFn(data, code), code, errorType);
	});
}

}

#endif // QTRESTCLIENT_HELPERTYPES_H
