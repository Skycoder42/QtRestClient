#-------------------------------------------------
#
# Project created by QtCreator 2017-03-05T23:26:44
#
#-------------------------------------------------

CONFIG += qrestbuilder

QT       += testlib

QT       -= gui

TARGET = tst_restbuilder
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../tests.pri)

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
	testmacro.h

SOURCES += tst_restbuilder.cpp

REST_API_FILES += \
	user.xml \
	post.xml \
	api_posts.xml \
	test_api.xml
