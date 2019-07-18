#ifndef QTRESTCLIENT_PAGINGMODEL_H
#define QTRESTCLIENT_PAGINGMODEL_H

#include <QtCore/qabstractitemmodel.h>
#include <QtCore/qscopedpointer.h>
#include <QtCore/qpointer.h>

#include "QtRestClient/qtrestclient_global.h"
#include "QtRestClient/ipaging.h"
#include "QtRestClient/restreply.h"
#include "QtRestClient/restclass.h"

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
#include "QtRestClient/paging_fwd.h"
#include "QtRestClient/genericrestreply.h"
#endif

namespace QtRestClient {

//! Interface for a component that fetches entries for the PagingModel
class Q_RESTCLIENT_EXPORT IPagingModelFetcher
{
	Q_DISABLE_COPY(IPagingModelFetcher)
public:
	IPagingModelFetcher();
	virtual ~IPagingModelFetcher();
	//! Returns the RestClient associated with this fetcher
	virtual RestClient *client() const = 0;
	//! Send a HTTP request to the given URL to obtain data for the model
	virtual RestReply *fetch(const QUrl &url) const = 0;
};

class PagingModelPrivate;
//! A Qt item model that fills itself from a Paging object obtained via the network
class Q_RESTCLIENT_EXPORT PagingModel : public QAbstractTableModel
{
	Q_OBJECT

	//! Holds the type the model fetches data for
	Q_PROPERTY(int typeId READ typeId NOTIFY typeIdChanged)

public:
	//! The Qt item role of the full REST-Object as was used to populate the model
	static constexpr int ModelDataRole = Qt::UserRole;

	//! Default constructor
	explicit PagingModel(QObject *parent = nullptr);
	~PagingModel() override;

	//! @copybrief PagingModel::initialize(const QUrl &, IPagingModelFetcher *)
	Q_INVOKABLE void initialize(const QUrl &initialUrl, QtRestClient::IPagingModelFetcher *fetcher, int typeId = QMetaType::QJsonValue);
	//! @copybrief PagingModel::initialize(const QUrl &, RestClass *)
	Q_INVOKABLE void initialize(const QUrl &initialUrl, QtRestClient::RestClass *restClass, int typeId = QMetaType::QJsonValue);
	//! Initialize the model using a type, a initial URL and a fetcher to use to send requests
	template <typename T>
	inline void initialize(const QUrl &initialUrl, IPagingModelFetcher *fetcher);
	//! @copybrief PagingModel::initialize(const QUrl &, IPagingModelFetcher *)
	template <typename T>
	inline void initialize(const QUrl &initialUrl, RestClass *restClass);

	// init methods from data
	//! @copybrief PagingModel::initialize(const Paging<T> &, IPagingModelFetcher *)
	Q_INVOKABLE void initialize(QtRestClient::RestReply *reply, QtRestClient::IPagingModelFetcher *fetcher, int typeId = QMetaType::QJsonValue);
	//! @copybrief PagingModel::initialize(const Paging<T> &, RestClass *)
	Q_INVOKABLE void initialize(QtRestClient::RestReply *reply, QtRestClient::RestClass *restClass, int typeId = QMetaType::QJsonValue);
	//! @copybrief PagingModel::initialize(GenericRestReply<Paging<DataClassType>, ErrorClassType> *, IPagingModelFetcher *)
	Q_INVOKABLE void initialize(QtRestClient::IPaging *paging, QtRestClient::IPagingModelFetcher *fetcher, int typeId = QMetaType::QJsonValue);
	//! @copybrief PagingModel::initialize(GenericRestReply<Paging<DataClassType>, ErrorClassType> *, RestClass *)
	Q_INVOKABLE void initialize(QtRestClient::IPaging *paging, QtRestClient::RestClass *restClass, int typeId = QMetaType::QJsonValue);
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	//! Initialize the model from a paging object and a fetcher to use to send requests
	template <typename T>
	inline void initialize(const Paging<T> &paging, IPagingModelFetcher *fetcher);
	//! @copybrief PagingModel::initialize(const Paging<T> &, IPagingModelFetcher *)
	template <typename T>
	inline void initialize(const Paging<T> &paging, RestClass *restClass);
	//! Initialize the model from a rest reply and a fetcher to use to send requests
	template <typename DataClassType, typename ErrorClassType = QObject*>
	inline void initialize(GenericRestReply<Paging<DataClassType>, ErrorClassType> *reply, IPagingModelFetcher *fetcher);
	//! @copybrief PagingModel::initialize(GenericRestReply<Paging<DataClassType>, ErrorClassType> *, IPagingModelFetcher *)
	template <typename DataClassType, typename ErrorClassType = QObject*>
	inline void initialize(GenericRestReply<Paging<DataClassType>, ErrorClassType> *reply, RestClass *restClass);
#endif

	//! @readAcFn{PagingModel::typeId}
	int typeId() const;

	//! @inherit{QAbstractTableModel::headerData}
	QVariant headerData(int section, Qt::Orientation orientation = Qt::Horizontal, int role = Qt::DisplayRole) const override;
	//! @inherit{QAbstractTableModel::rowCount}
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	//! @inherit{QAbstractTableModel::columnCount}
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	//! @inherit{QAbstractTableModel::canFetchMore}
	bool canFetchMore(const QModelIndex &parent) const override;
	//! @inherit{QAbstractTableModel::fetchMore}
	void fetchMore(const QModelIndex &parent) override;

	//! @inherit{QAbstractTableModel::data}
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	//! Returns the object at the given index
	Q_INVOKABLE QVariant object(const QModelIndex &index) const;
	/*! @copybrief PagingModel::object(const QModelIndex &) const
	 * @tparam T The type of object to return. Must match PagingModel::typeId
	 * @copydetails PagingModel::object(const QModelIndex &) const
	 */
	template <typename T>
	inline T object(const QModelIndex &index) const;

	//! @inherit{QAbstractTableModel::flags}
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	//! @inherit{QAbstractTableModel::roleNames}
	QHash<int, QByteArray> roleNames() const override;

	//! Add a new column with the given title
	int addColumn(const QString &text);
	//! Add a new column and set the given property as its Qt::DisplayRole
	int addColumn(const QString &text, const char *propertyName);
	//! Adds the given property as a new role to the given column
	void addRole(int column, int role, const char *propertyName);
	//! Removes all customly added columns and roles
	void clearColumns();

Q_SIGNALS:
	//! Gets emitted if the model fails to obtain data via the network
	void fetchError(QPrivateSignal);

	//! @notifyAcFn{PagingModel::typeId}
	void typeIdChanged(int typeId, QPrivateSignal);

private:
	friend class PagingModelPrivate;
	QScopedPointer<PagingModelPrivate> d;
};

//! A default implementation for a IPagingModelFetcher, using a RestClass to send the requests
class Q_RESTCLIENT_EXPORT RestClassFetcher : public IPagingModelFetcher
{
public:
	//! Default constructor
	RestClassFetcher(RestClass *restClass);
	RestClient *client() const override;
	RestReply *fetch(const QUrl &url) const override;

private:
	QPointer<RestClass> _restClass;
};

// ------------- Generic implementation -------------

template<typename T>
inline void PagingModel::initialize(const QUrl &initialUrl, IPagingModelFetcher *fetcher)
{
	initialize(initialUrl, fetcher, qMetaTypeId<T>());
}

template<typename T>
inline void PagingModel::initialize(const QUrl &initialUrl, RestClass *restClass)
{
	initialize(initialUrl, restClass, qMetaTypeId<T>());
}

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
template<typename T>
inline void PagingModel::initialize(const Paging<T> &paging, IPagingModelFetcher *fetcher)
{
	initialize(paging.iPaging(), fetcher, qMetaTypeId<T>());
}

template<typename T>
inline void PagingModel::initialize(const Paging<T> &paging, RestClass *restClass)
{
	initialize(paging.iPaging(), restClass, qMetaTypeId<T>());
}

template<typename DataClassType, typename ErrorClassType>
inline void PagingModel::initialize(GenericRestReply<Paging<DataClassType>, ErrorClassType> *reply, IPagingModelFetcher *fetcher)
{
	initialize(reply, fetcher, qMetaTypeId<DataClassType>());
}

template<typename DataClassType, typename ErrorClassType>
inline void PagingModel::initialize(GenericRestReply<Paging<DataClassType>, ErrorClassType> *reply, RestClass *restClass)
{
	initialize(reply, restClass, qMetaTypeId<DataClassType>());
}
#endif

template <typename T>
inline T PagingModel::object(const QModelIndex &index) const
{
	Q_ASSERT_X(qMetaTypeId<T>() == typeId(), Q_FUNC_INFO, "object must be used with the stores typeId");
	return object(index).value<T>();
}

}

Q_DECLARE_METATYPE(QtRestClient::IPagingModelFetcher*)

#endif // QTRESTCLIENT_PAGINGMODEL_H
