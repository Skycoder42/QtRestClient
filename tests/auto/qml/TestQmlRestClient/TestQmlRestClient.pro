TEMPLATE = app
TARGET = tst_qmlrestclient
CONFIG += qmltestcase console
SOURCES += tst_qmlrestclient.cpp

QT += restclient

importFiles.path = .
DEPLOYMENT += importFiles

DISTFILES += \
	tst_qmlrestclient.qml

QML_IMPORT_PATH = $$OUT_PWD/../../../../qml/

DEFINES += SRCDIR=\\\"$$_PRO_FILE_PWD_/\\\"

DEFINES += QML_PATH=\\\"$$QML_IMPORT_PATH\\\"

mac: QMAKE_LFLAGS += '-Wl,-rpath,\'$$OUT_PWD/../../../../lib\''
