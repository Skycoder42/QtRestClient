#-------------------------------------------------
#
# Project created by QtCreator 2016-12-31T10:29:52
#
#-------------------------------------------------

QT       += network testlib

QT       -= gui

TARGET = tst_requestbuilder
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += tst_requestbuilder.cpp \
	main.cpp \
    tst_restclient.cpp

HEADERS += \
    tst_requestbuilder.h \
    tst_restclient.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QtRestClient/release/ -lQtRestClient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QtRestClient/debug/ -lQtRestClient
else:mac: LIBS += -F$$OUT_PWD/../QtRestClient/ -framework QtRestClient
else:unix: LIBS += -L$$OUT_PWD/../QtRestClient/ -lQtRestClient

INCLUDEPATH += $$PWD/../QtRestClient
DEPENDPATH += $$PWD/../QtRestClient
