TEMPLATE = app

QT += testlib restclient-private
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_restreply

include(../tests.pri)

SOURCES += tst_restreply.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
