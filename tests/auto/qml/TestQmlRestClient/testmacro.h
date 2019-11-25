#ifndef TESTMACRO_H
#define TESTMACRO_H

#include <QtGlobal>
#include <QUrl>

#define TEST_EXPORT
#define LIMIT 100
#define APP_SECRET "baum42"

namespace TestSvr {
QUrl getUrl();
}

#endif // TESTMACRO_H
