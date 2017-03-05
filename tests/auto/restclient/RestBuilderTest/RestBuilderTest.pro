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

DEFINES += QT_DEPRECATED_WARNINGS

include(../tests.pri)

SOURCES += tst_restbuilder.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

REST_API_CLASSES += test_api.json
REST_API_OBJECTS += Post.json \
	User.json

DISTFILES += \
	test_api.json \
	Post.json \
	User.json
