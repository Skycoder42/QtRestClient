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

include(../tests2.pri)

SOURCES += tst_requestbuilder.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
