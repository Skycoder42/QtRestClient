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

class Q_RESTCLIENT_EXPORT PagingModelFetcher
{
	Q_DISABLE_COPY(PagingModelFetcher)
public:
	PagingModelFetcher();
	virtual ~PagingModelFetcher();
	virtual RestClient *client() const = 0;
	virtual RestReply *fetch(const QUrl &url) const = 0;
};

class PagingModelPrivate;
class Q_RESTCLIENT_EXPORT PagingModel : public QAbstractTableModel
{
	Q_OBJECT

	//! Holds the type the model loads data for
	Q_PROPERTY(int typeId READ typeId NOTIFY typeIdChanged)

public:
	static constexpr int ModelDataRole = Qt::UserRole;

	explicit PagingModel(QObject *parent = nullptr);
	~PagingModel() override;

	// direct init methods
	Q_INVOKABLE void initialize(const QUrl &initialUrl, QtRestClient::PagingModelFetcher *fetcher, int typeId = QMetaType::QJsonValue);
	Q_INVOKABLE void initialize(const QUrl &initialUrl, QtRestClient::RestClass *restClass, int typeId = QMetaType::QJsonValue);
	template <typename T>
	inline void initialize(const QUrl &initialUrl, PagingModelFetcher *fetcher);
	template <typename T>
	inline void initialize(const QUrl &initialUrl, RestClass *restClass);

	// init methods from data
	Q_INVOKABLE void initialize(QtRestClient::RestReply *reply, QtRestClient::PagingModelFetcher *fetcher, int typeId = QMetaType::QJsonValue);
	Q_INVOKABLE void initialize(QtRestClient::RestReply *reply, QtRestClient::RestClass *restClass, int typeId = QMetaType::QJsonValue);
	Q_INVOKABLE void initialize(QtRestClient::IPaging *paging, QtRestClient::PagingModelFetcher *fetcher, int typeId = QMetaType::QJsonValue);
	Q_INVOKABLE void initialize(QtRestClient::IPaging *paging, QtRestClient::RestClass *restClass, int typeId = QMetaType::QJsonValue);
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	template <typename T>
	inline void initialize(const Paging<T> &paging, PagingModelFetcher *fetcher);
	template <typename T>
	inline void initialize(const Paging<T> &paging, RestClass *restClass);
	template <typename DataClassType, typename ErrorClassType = QObject*>
	inline void initialize(GenericRestReply<Paging<DataClassType>, ErrorClassType> *reply, PagingModelFetcher *fetcher);
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
	void fetchError(QPrivateSignal);

	//! @notifyAcFn{PagingModel::typeId}
	void typeIdChanged(int typeId, QPrivateSignal);

private:
	friend class PagingModelPrivate;
	QScopedPointer<PagingModelPrivate> d;
};

class Q_RESTCLIENT_EXPORT RestClassFetcher : public PagingModelFetcher
{
public:
	RestClassFetcher(RestClass *restClass);
	RestClient *client() const override;
	RestReply *fetch(const QUrl &url) const override;

private:
	QPointer<RestClass> _restClass;
};

// ------------- Generic implementation -------------

template<typename T>
inline void PagingModel::initialize(const QUrl &initialUrl, PagingModelFetcher *fetcher)
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
inline void PagingModel::initialize(const Paging<T> &paging, PagingModelFetcher *fetcher)
{
	initialize(paging.iPaging(), fetcher, qMetaTypeId<T>());
}

template<typename T>
inline void PagingModel::initialize(const Paging<T> &paging, RestClass *restClass)
{
	initialize(paging.iPaging(), restClass, qMetaTypeId<T>());
}

template<typename DataClassType, typename ErrorClassType>
inline void PagingModel::initialize(GenericRestReply<Paging<DataClassType>, ErrorClassType> *reply, PagingModelFetcher *fetcher)
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

Q_DECLARE_METATYPE(QtRestClient::PagingModelFetcher*)

#endif // QTRESTCLIENT_PAGINGMODEL_H
