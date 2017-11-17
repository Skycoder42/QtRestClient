#-------------------------------------------------
#
# Project created by QtCreator 2016-12-31T15:58:38
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_integration
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../tests2.pri)

SOURCES += tst_integration.cpp \
	jphuser.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
	jphuser.h
