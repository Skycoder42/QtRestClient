#ifndef RESTCLASS_P_H
#define RESTCLASS_P_H

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

#endif // RESTCLASS_P_H
