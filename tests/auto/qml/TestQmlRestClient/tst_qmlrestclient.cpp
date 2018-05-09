#include <QtCore>
#include <QtQuickTest/quicktest.h>

static void initImportPath()
{
#ifdef Q_OS_LINUX
	if(!qgetenv("LD_PRELOAD").contains("Qt5RestClient"))
		qWarning() << "No LD_PRELOAD set - this may fail on systems with multiple version of the modules";
#endif
	qputenv("QML2_IMPORT_PATH", QML_PATH);
}
Q_COREAPP_STARTUP_FUNCTION(initImportPath)

QUICK_TEST_MAIN(qmlrestclient)
