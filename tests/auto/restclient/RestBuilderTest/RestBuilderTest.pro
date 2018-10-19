TEMPLATE = app

QT += testlib
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_restbuilder

include(../tests.pri)

HEADERS += \
	testmacro.h

SOURCES += tst_restbuilder.cpp

REST_API_FILES += \
	user.xml \
	post.xml \
	api_posts.xml \
	test_api.xml \
	simplepost.xml

DEFINES += SRCDIR=\\\"$$PWD/\\\"
