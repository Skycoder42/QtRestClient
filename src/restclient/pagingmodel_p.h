#ifndef QTRESTCLIENT_PAGINGMODEL_P_H
#define QTRESTCLIENT_PAGINGMODEL_P_H

#include "pagingmodel.h"

#include <optional>

#include <QtCore/QHash>
#include <QtCore/QLoggingCategory>

#include <QtCore/private/qabstractitemmodel_p.h>

namespace QtRestClient {

class Q_RESTCLIENT_EXPORT PagingModelPrivate : public QAbstractItemModelPrivate
{
	Q_DECLARE_PUBLIC(PagingModel)
public:
	int typeId = QMetaType::UnknownType;
	QScopedPointer<IPagingModelFetcher> fetcher {};
	std::optional<QUrl> nextUrl;
	QVariantList data;

	QHash<int, QByteArray> pagingRoleNames;
	QStringList columns;
	QHash<int, QHash<int, QByteArray>> roleMapping; //column -> (role -> property)

	void clearData();
	void generateRoleNames();
	void requestNext();
	void processReply(int code, const RestReply::DataType &data);
	void processPaging(IPaging *paging);
	void processError(const QString &message, int code, RestReply::Error errorType);
};

Q_DECLARE_LOGGING_CATEGORY(logPagingModel)

}

#endif // QTRESTCLIENT_PAGINGMODEL_P_H
