TEMPLATE = app

QT += testlib restclientauth
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_authrequestbuilder

LIB_PWD = $$OUT_PWD/../../restclient/testlib
include(../../restclient/tests.pri)

SOURCES += \
	tst_authrequestbuilder.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
