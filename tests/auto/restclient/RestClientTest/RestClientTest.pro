TEMPLATE = app

QT += testlib
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_restclient

include(../tests.pri)

SOURCES += tst_restclient.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
