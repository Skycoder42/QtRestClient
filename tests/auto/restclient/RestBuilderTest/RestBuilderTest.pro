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

include(../tests2.pri)

SOURCES += tst_restbuilder.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

REST_API_OBJECTS += post.json \
	user.json
REST_API_CLASSES += test_api.json \
	api_posts.json

HEADERS += \
	testmacro.h
