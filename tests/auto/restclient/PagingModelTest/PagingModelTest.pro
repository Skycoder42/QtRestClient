TEMPLATE = app

QT += testlib gui widgets

TARGET = tst_pagingmodel

include(../tests.pri)

SOURCES += \
	tst_pagingmodel.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
