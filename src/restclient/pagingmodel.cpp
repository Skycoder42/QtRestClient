#include "pagingmodel.h"
#include "pagingmodel_p.h"
#include <QtCore/QMetaProperty>
#include <QtCore/QDebug>
using namespace QtRestClient;
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
using namespace QtJsonSerializer;
#endif
namespace sph = std::placeholders;

PagingModel::PagingModel(QObject *parent) :
	  PagingModel{*new PagingModelPrivate{}, parent}
{}

void PagingModel::initialize(const QUrl &initialUrl, IPagingModelFetcher *fetcher, int typeId)
{
	Q_D(PagingModel);
	beginResetModel();
	d->typeId = typeId;
	d->fetcher.reset(fetcher);
	d->nextUrl = initialUrl;
	d->data.clear();
	d->generateRoleNames();
	endResetModel();  // automatically calls fetchMore
}

void PagingModel::initialize(const QUrl &initialUrl, RestClass *restClass, int typeId)
{
	initialize(initialUrl, new RestClassFetcher{restClass}, typeId);
}

void PagingModel::initialize(RestReply *reply, IPagingModelFetcher *fetcher, int typeId)
{
	Q_D(PagingModel);
	beginResetModel();
	d->typeId = typeId;
	d->fetcher.reset(fetcher);
	d->data.clear();
	d->generateRoleNames();
	endResetModel();
	reply->onSucceeded(this, std::bind(&PagingModelPrivate::processReply, d, sph::_1, sph::_2));
	reply->onAllErrors(this, std::bind(&PagingModelPrivate::processError, d, sph::_1, sph::_2, sph::_3));
}

void PagingModel::initialize(RestReply *reply, RestClass *restClass, int typeId)
{
	initialize(reply, new RestClassFetcher{restClass}, typeId);
}

void PagingModel::initialize(IPaging *paging, IPagingModelFetcher *fetcher, int typeId)
{
	Q_D(PagingModel);
	beginResetModel();
	d->typeId = typeId;
	d->fetcher.reset(fetcher);
	d->data.clear();
	d->generateRoleNames();
	endResetModel();
	d->processPaging(paging);
}

void PagingModel::initialize(IPaging *paging, RestClass *restClass, int typeId)
{
	initialize(paging, new RestClassFetcher{restClass}, typeId);
}

int PagingModel::typeId() const
{
	Q_D(const PagingModel);
	return d->typeId;
}

QVariant PagingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_D(const PagingModel);
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
		return {};

	if (d->columns.isEmpty()) {
		if (section == 0) {
			auto metaObject = QMetaType::metaObjectForType(d->typeId);
			if (metaObject)
				return QString::fromUtf8(metaObject->className());
		}
	} else if (section < d->columns.size())
		return d->columns.value(section);

	return {};
}

int PagingModel::rowCount(const QModelIndex &parent) const
{
	Q_D(const PagingModel);
	Q_ASSERT(checkIndex(parent, CheckIndexOption::DoNotUseParent));
	if (parent.isValid())
		return 0;
	else
		return d->data.size();
}

int PagingModel::columnCount(const QModelIndex &parent) const
{
	Q_D(const PagingModel);
	Q_ASSERT(checkIndex(parent, CheckIndexOption::DoNotUseParent));
	if (parent.isValid())
		return 0;
	else
		return d->columns.isEmpty() ? 1 : d->columns.size();
}

bool PagingModel::canFetchMore(const QModelIndex &parent) const
{
	Q_D(const PagingModel);
	Q_ASSERT(checkIndex(parent, CheckIndexOption::DoNotUseParent));
	if (parent.isValid())
		return false;
	else
		return d->nextUrl.has_value();
}

void PagingModel::fetchMore(const QModelIndex &parent)
{
	Q_D(PagingModel);
	if (!canFetchMore(parent))
		return;
	d->requestNext();
}

QVariant PagingModel::data(const QModelIndex &index, int role) const
{
	Q_D(const PagingModel);
	Q_ASSERT(checkIndex(index, CheckIndexOption::ParentIsInvalid | CheckIndexOption::IndexIsValid));

	// handle model data
	if (role == ModelDataRole)
		return d->data.at(index.row());

	// get the role name
	QByteArray pName;
	if (!d->columns.isEmpty()) {
		const auto &subRoles = d->roleMapping[index.column()];
		pName = subRoles.value(role);
	}

	if (pName.isEmpty() && role != Qt::DisplayRole)
		pName = d->pagingRoleNames.value(role);

	// get the actual data
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	if (d->typeId == QMetaType::UnknownType) {
#endif
		// handle special case: json value (only mode available without the serializer)
		const auto value = d->data.at(index.row());
		if (!pName.isEmpty())
			return value.toMap().value(QString::fromUtf8(pName));
		else if (role == Qt::DisplayRole) {
			if (value.userType() == QMetaType::QVariantMap)
				return QStringLiteral("Object <%L1>").arg(index.row());
			else if (value.userType() == QMetaType::QVariantList)
				return QStringLiteral("Array <%L1>").arg(index.row());
			else
				return value;
		} else
			return {};
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	} else {
		// get the meta object
		const auto metaObject = QMetaType::metaObjectForType(d->typeId);
		if (!metaObject)
			return {};

		// find the property name
		if (pName.isEmpty() && role == Qt::DisplayRole) {
			const auto userProp = metaObject->userProperty();
			if (userProp.isValid())
				pName = userProp.name();
			else
				return QStringLiteral("%1 <%L2>").arg(QString::fromUtf8(metaObject->className()), index.row());
		}

		// obtain the meta property
		const auto pIndex = metaObject->indexOfProperty(pName.constData());
		if(pIndex == -1)
			return {};
		const auto prop = metaObject->property(pIndex);

		// read the value
		if (metaObject->inherits(&QObject::staticMetaObject))
			return prop.read(d->data.at(index.row()).value<QObject*>());
		else
			return prop.readOnGadget(d->data.at(index.row()).data());
	}
#endif
}

QVariant PagingModel::object(const QModelIndex &index) const
{
	Q_D(const PagingModel);
	Q_ASSERT(checkIndex(index, CheckIndexOption::ParentIsInvalid | CheckIndexOption::IndexIsValid));
	return d->data.at(index.row());
}

Qt::ItemFlags PagingModel::flags(const QModelIndex &index) const
{
	Q_ASSERT(checkIndex(index, CheckIndexOption::ParentIsInvalid));
	if (!index.isValid())
		return Qt::NoItemFlags;
	else
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren;
}

QHash<int, QByteArray> PagingModel::roleNames() const
{
	Q_D(const PagingModel);
	return d->pagingRoleNames;
}

int PagingModel::addColumn(const QString &text)
{
	Q_D(PagingModel);
	const auto index = d->columns.size();
	beginInsertColumns(QModelIndex{}, index, index);
	d->columns.append(text);
	endInsertColumns();
	emit headerDataChanged(Qt::Horizontal, index, index);
	return index;
}

int PagingModel::addColumn(const QString &text, const char *propertyName)
{
	const auto index = addColumn(text);
	addRole(index, Qt::DisplayRole, propertyName);
	return index;
}

void PagingModel::addRole(int column, int role, const char *propertyName)
{
	Q_D(PagingModel);
	Q_ASSERT_X(column < d->columns.size(), Q_FUNC_INFO, "Cannot add role to non existant column!");
	d->roleMapping[column].insert(role, propertyName);
	if (!d->data.isEmpty())
		emit dataChanged(this->index(0, column), this->index(d->data.size() - 1, column), {role});
}

void PagingModel::clearColumns()
{
	Q_D(PagingModel);
	const auto cColumns = d->columns.size() > 1;
	if (cColumns)
		beginRemoveColumns({}, 1, d->columns.size() - 1);
	d->columns.clear();
	d->roleMapping.clear();
	if (cColumns)
		endRemoveColumns();
	if (!d->data.isEmpty())
		emit dataChanged(this->index(0, 0), this->index(d->data.size() - 1, 0));
}

PagingModel::PagingModel(PagingModelPrivate &dd, QObject *parent) :
	  QAbstractTableModel{dd, parent}
{}



IPagingModelFetcher::IPagingModelFetcher() = default;

IPagingModelFetcher::~IPagingModelFetcher() = default;



RestClassFetcher::RestClassFetcher(RestClass *restClass) :
	_restClass{restClass}
{}

RestClient *RestClassFetcher::client() const
{
	return _restClass ? _restClass->client() : nullptr;
}

RestReply *RestClassFetcher::fetch(const QUrl &url) const
{
	return _restClass ? _restClass->callRaw(RestClass::GetVerb, url) : nullptr;
}

// ------------- Private Implementation -------------

void PagingModelPrivate::generateRoleNames()
{
	pagingRoleNames = {{PagingModel::ModelDataRole, "modelData"}};
	const auto metaObject = QMetaType::metaObjectForType(typeId);
	if (metaObject) {
		auto roleIndex = PagingModel::ModelDataRole;
		for(auto i = 0; i < metaObject->propertyCount(); ++i)
			pagingRoleNames.insert(++roleIndex, metaObject->property(i).name());
	}
}

void PagingModelPrivate::requestNext()
{
	Q_Q(PagingModel);
	Q_ASSERT(nextUrl);
	auto reply = fetcher->fetch(*nextUrl);
	if (reply) {
		nextUrl = std::nullopt;
		reply->onSucceeded(q, std::bind(&PagingModelPrivate::processReply, this, sph::_1, sph::_2));
		reply->onAllErrors(q, std::bind(&PagingModelPrivate::processError, this, sph::_1, sph::_2, sph::_3));
	} else
		emit q->fetchError({});
}

void PagingModelPrivate::processReply(int, const RestReply::DataType &data)
{
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
	Q_Q(PagingModel);
	IPaging *paging;
	try {
		paging = fetcher->client()->pagingFactory()->createPaging(fetcher->client()->serializer(), jsonData);
	} catch (Exception &e) {
		qCritical() << "Failed to parse received paging object with error:"
				   << e.what();
		emit q->fetchError({});
		return;
	}
#else
	auto paging = std::visit(__private::overload {
								 [](std::nullopt_t) -> IPaging* {
									 return nullptr;
								 },
								 [this](auto vData) -> IPaging* {
									 return fetcher->client()->pagingFactory()->createPaging(vData);
								 }
							 }, data);
#endif

	if (paging) {
		processPaging(paging);
		delete paging;
	}
}

void PagingModelPrivate::processPaging(IPaging *paging)
{
	Q_Q(PagingModel);
	if (paging->offset() < data.size()) {
		qWarning() << "Pagings out of sync - dropping duplicate data";
		data = data.mid(0, static_cast<int>(paging->offset()));
	} else if (paging->offset() > data.size()) {
		if (paging->hasPrevious()) {
			qWarning() << "Pagings out of sync - trying for previous data";
			nextUrl = paging->previous();
			requestNext();
			return;
		} else {
			qWarning() << "Pagings out of sync - skipping" << paging->offset() - data.size()
					   << "unobtainable elements";
		}
	}

	std::visit([&](const auto &items){
		q->beginInsertRows({}, data.size(), static_cast<int>(data.size() + items.size() - 1));
#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
		const auto serializer = fetcher->client()->serializer();
		auto fetchFailed = false;
		for (const auto &item : items) {
			try {
				data.append(serializer->deserializeGeneric(item, typeId, q));  // TODO care about memory leak
			} catch (DeserializationException &e) {
				qCritical() << "Failed to deserialize paging element with error:"
							<< e.what();
				fetchFailed = true;
			}
		}
#else
		for (const auto &item : items)
			data.append(item.toVariant());
#endif

		if (data.size() < paging->total() && paging->hasNext())
			nextUrl = paging->next();
		else
			nextUrl = std::nullopt;
		q->endInsertRows();

#ifndef Q_RESTCLIENT_NO_JSON_SERIALIZER
		if (fetchFailed)
			emit q->fetchError({});
#endif
	}, paging->items());
}

void PagingModelPrivate::processError(const QString &message, int code, RestReply::Error errorType)
{
	Q_Q(PagingModel);
	qCritical() << "Network request failed with error of type" << errorType
				<< "and code" << code << "- error message is:" << qUtf8Printable(message);
	emit q->fetchError({});
}
