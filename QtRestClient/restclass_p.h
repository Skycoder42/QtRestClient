#ifndef RESTCLASS_P_H
#define RESTCLASS_P_H

#include "restclass.h"

namespace QtRestClient {

class RestClassPrivate
{
public:
	RestClient *client;
	QStringList subPath;

	static QUrlQuery hashToQuery(const QVariantHash &hash);

	RestClassPrivate(RestClient *client, QStringList subPath);
};

}

#endif // RESTCLASS_P_H
