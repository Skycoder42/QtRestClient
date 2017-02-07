#-------------------------------------------------
#
# Project created by QtCreator 2016-12-29T23:25:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

TARGET = SimpleRestClientApp
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


SOURCES += main.cpp\
		mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

include(../QJsonSerializer/qjsonserializer.pri)

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QtRestClient/release/ -lQtRestClient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QtRestClient/debug/ -lQtRestClient
else:mac: LIBS += -F$$OUT_PWD/../QtRestClient/ -framework QtRestClient
else:unix: LIBS += -L$$OUT_PWD/../QtRestClient/ -lQtRestClient

INCLUDEPATH += $$PWD/../QtRestClient
DEPENDPATH += $$PWD/../QtRestClient
