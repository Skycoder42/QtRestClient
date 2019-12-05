#ifndef RESTREPLYAWAITABLE_P_H
#define RESTREPLYAWAITABLE_P_H

#include "restreplyawaitable.h"

#include <QtCore/QPointer>

namespace QtRestClient {

class RestReplyAwaitablePrivate
{
	Q_DISABLE_COPY(RestReplyAwaitablePrivate)
public:
	RestReplyAwaitablePrivate() = default;

	QPointer<RestReply> reply{};

	RestReply::DataType successResult = std::nullopt;
	QScopedPointer<AwaitedException> errorResult{};
};

}

#endif // RESTREPLYAWAITABLE_P_H
