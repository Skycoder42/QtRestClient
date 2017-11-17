#-------------------------------------------------
#
# Project created by QtCreator 2016-12-31T15:14:24
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_restclient
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../tests2.pri)

SOURCES += tst_restclient.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
