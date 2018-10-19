TEMPLATE = app

QT += testlib
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_restreply

include(../tests.pri)

HEADERS += \
	simplejphpost.h

SOURCES += tst_restreply.cpp \
	simplejphpost.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
