TEMPLATE = app

QT += testlib
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = tst_restawaitables

include(../tests.pri)

SOURCES += \
	tst_restawaitables.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

QDEP_DEPENDS += Skycoder42/QtCoroutines

!load(qdep):warning("Failed to load qdep feature! Run 'qdep.py prfgen --qmake $$QMAKE_QMAKE' to create it.")
