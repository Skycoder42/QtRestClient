#-------------------------------------------------
#
# Project created by QtCreator 2016-12-31T15:24:19
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_restreply
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(../tests.pri)

SOURCES += tst_restreply.cpp \
	simplejphpost.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
	simplejphpost.h
