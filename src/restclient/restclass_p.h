#ifndef QTRESTCLIENT_RESTCLASS_P_H
#define QTRESTCLIENT_RESTCLASS_P_H

#include "restclass.h"

#include <QtCore/private/qobject_p.h>

namespace QtRestClient {

class Q_RESTCLIENT_EXPORT RestClassPrivate : public QObjectPrivate
{
	Q_DECLARE_PUBLIC(RestClass)
public:
	RestClient *client;
	QStringList subPath;

	static QUrlQuery hashToQuery(const QVariantHash &hash);
};

}

#endif // QTRESTCLIENT_RESTCLASS_P_H
