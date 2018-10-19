TEMPLATE = app

QT += testlib
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_restawaitables

include(../tests.pri)

SOURCES += \
	tst_restawaitables.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
