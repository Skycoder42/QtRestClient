#ifndef QTRESTCLIENT_GLOBAL_H
#define QTRESTCLIENT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QTRESTCLIENT_LIBRARY)
#  define QTRESTCLIENTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QTRESTCLIENTSHARED_EXPORT Q_DECL_IMPORT
#endif

namespace QtRestClient {

typedef QHash<QByteArray, QByteArray> HeaderHash;

}

#endif // QTRESTCLIENT_GLOBAL_H
