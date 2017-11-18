TEMPLATE = lib

QT += testlib restclient
QT -= gui

CONFIG += static

TARGET = testlib

DEFINES += TESTLIB_LIBRARY
DEFINES += "TESTLIB_SRC_DIR=\\\"$$PWD\\\""

HEADERS += \
	testlib.h \
	jphpost.h \
	httpserver.h

SOURCES += \
	testlib.cpp \
	jphpost.cpp \
	httpserver.cpp
