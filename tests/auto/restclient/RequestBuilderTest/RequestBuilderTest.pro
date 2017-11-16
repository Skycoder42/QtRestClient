#-------------------------------------------------
#
# Project created by QtCreator 2016-12-31T15:08:07
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_requestbuilder
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#include(../tests.pri)
QT += core network qml jsonserializer restclient
DEFINES -= QT_ASCII_CAST_WARNINGS

SOURCES += tst_requestbuilder.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../testlib/release/ -ltestlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../testlib/debug/ -ltestlib
else:unix: LIBS += -L$$OUT_PWD/../testlib/ -ltestlib

INCLUDEPATH += $$PWD/../testlib
DEPENDPATH += $$PWD/../testlib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../testlib/release/libtestlib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../testlib/debug/libtestlib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../testlib/release/testlib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../testlib/debug/testlib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../testlib/libtestlib.a
