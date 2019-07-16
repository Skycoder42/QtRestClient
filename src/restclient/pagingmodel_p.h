#ifndef QTRESTCLIENT_PAGINGMODEL_P_H
#define QTRESTCLIENT_PAGINGMODEL_P_H

#include "pagingmodel.h"

#include <optional>

#include <QtCore/QHash>

namespace QtRestClient {

class Q_RESTCLIENT_EXPORT PagingModelPrivate
{
	Q_DISABLE_COPY(PagingModelPrivate)

public:
	PagingModelPrivate(PagingModel *q_ptr);

	PagingModel *q;

	int typeId = QMetaType::UnknownType;
	QScopedPointer<PagingModelFetcher> fetcher {};
	std::optional<QUrl> nextUrl;
	QVariantList data;

	QHash<int, QByteArray> roleNames;
	QStringList columns;
	QHash<int, QHash<int, QByteArray>> roleMapping; //column -> (role -> property)

	void generateRoleNames();
	void requestNext();
	void processReply(int code, const QJsonObject &jsonData);
	void processPaging(IPaging *paging);
	void processError(const QString &message, int code, RestReply::ErrorType errorType);
};

}

#endif // QTRESTCLIENT_PAGINGMODEL_P_H
