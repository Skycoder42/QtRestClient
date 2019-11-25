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

DEFINES += SRCDIR=\\\"$$_PRO_FILE_PWD_/\\\"

LIB_PWD = $$OUT_PWD/../../restclient/testlib
include(../../restclient/tests.pri)

win32:msvc:CONFIG(debug, debug|release) {
	QMAKE_EXTRA_TARGETS -= runtarget
	runtarget_dummy.target = run-tests
	QMAKE_EXTRA_TARGETS += runtarget_dummy
}
