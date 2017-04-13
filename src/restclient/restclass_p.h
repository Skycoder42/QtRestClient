#ifndef QTRESTCLIENT_RESTCLASS_P_H
#define QTRESTCLIENT_RESTCLASS_P_H

#include "restclass.h"

namespace QtRestClient {

class Q_RESTCLIENT_EXPORT RestClassPrivate
{
public:
	RestClient *client;
	QStringList subPath;

	static QUrlQuery hashToQuery(const QVariantHash &hash);

	RestClassPrivate(RestClient *client, QStringList subPath);
};

}

#endif // QTRESTCLIENT_RESTCLASS_P_H
