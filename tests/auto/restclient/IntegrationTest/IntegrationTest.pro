TEMPLATE = app

QT += testlib
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_integration

include(../tests.pri)

HEADERS += \
	jphuser.h

SOURCES += tst_integration.cpp \
	jphuser.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
