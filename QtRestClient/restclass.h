#ifndef RESTCLASS_H
#define RESTCLASS_H

#include "qtrestclient_global.h"
#include "requestbuilder.h"
#include "restreply.h"

#include <QObject>

namespace QtRestClient {

class RestClient;

class RestClassPrivate;
class QTRESTCLIENTSHARED_EXPORT RestClass : public QObject
{
	Q_OBJECT
	friend class RestClient;

public:
	~RestClass();

	RestClass *subClass(const QString &path, QObject *parent = nullptr);

//	void* get(const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
//	void* get(const QString &methodPath, QJsonObject body, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
//	void* get(const QString &methodPath, QJsonArray body, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
//	template<typename... Args>
//	void* get(const QString &methodPath, Args... parameters);
//	template<typename T, typename... Args>
//	void* get(const QString &methodPath, const T &body, Args... parameters);
//	template<typename... Args>
//	void* get(const QString &methodPath, const HeaderHash & headers, Args... parameters);
//	template<typename T, typename... Args>
//	void* get(const QString &methodPath, const T &body, const HeaderHash & headers, Args... parameters);

	RestReply* call(QByteArray verb, const QString &methodPath, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
	RestReply* call(QByteArray verb,const QString &methodPath, QJsonObject body, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
	RestReply* call(QByteArray verb,const QString &methodPath, QJsonArray body, const QVariantHash &parameters = {}, const HeaderHash & headers = {});
//	template<typename... Args>
//	void* call(QByteArray verb,const QString &methodPath, Args... parameters);
//	template<typename T, typename... Args>
//	void* call(QByteArray verb,const QString &methodPath, const T &body, Args... parameters);
//	template<typename... Args>
//	void* call(QByteArray verb,const QString &methodPath, const HeaderHash & headers, Args... parameters);
//	template<typename T, typename... Args>
//	void* call(QByteArray verb,const QString &methodPath, const T &body, const HeaderHash & headers, Args... parameters);

	RequestBuilder builder() const;

private:
	QScopedPointer<RestClassPrivate> d_ptr;

	explicit RestClass(RestClient *client, QStringList subPath, QObject *parent);
};

}

#endif // RESTCLASS_H
