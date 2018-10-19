TEMPLATE = app

QT += restclient
CONFIG += qmltestcase console

TARGET = tst_qmlrestclient

HEADERS += \
	testmacro.h

SOURCES += tst_qmlrestclient.cpp

REST_API_FILES += \
	user.xml \
	post.xml \
	api_posts.xml \
	test_api.xml \
	simplepost.xml

importFiles.path = .
DEPLOYMENT += importFiles

DISTFILES += \
	tst_qmlrestclient.qml

QML_IMPORT_PATH = $$OUT_PWD/../../../../qml/
DEFINES += SRCDIR=\\\"$$_PRO_FILE_PWD_/\\\"
DEFINES += QML_PATH=\\\"$$QML_IMPORT_PATH\\\"

LIB_PWD = $$OUT_PWD/../../restclient/testlib
include(../../restclient/tests.pri)
