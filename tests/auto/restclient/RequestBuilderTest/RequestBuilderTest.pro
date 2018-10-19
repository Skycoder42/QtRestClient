TEMPLATE = app

QT += testlib
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_requestbuilder

include(../tests.pri)

SOURCES += tst_requestbuilder.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
