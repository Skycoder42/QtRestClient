TEMPLATE = app

QT += restclient
CONFIG += qmltestcase console

TARGET = tst_qmlrestclient

CONFIG(release, debug|release): DEFINES += "TEST_PORT=38723"
CONFIG(debug, debug|release): DEFINES += "TEST_PORT=38724"

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

win32:msvc:CONFIG(debug, debug|release) {
	QMAKE_EXTRA_TARGETS -= runtarget
	runtarget_dummy.target = run-tests
	QMAKE_EXTRA_TARGETS += runtarget_dummy
}
