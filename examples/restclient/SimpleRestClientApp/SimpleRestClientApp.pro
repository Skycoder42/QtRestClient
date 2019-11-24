TEMPLATE = app

QT += core gui widgets restclient network

TARGET = SimpleRestClientApp

SOURCES += main.cpp \
	mainwindow.cpp

HEADERS += mainwindow.h

FORMS += mainwindow.ui

target.path = $$[QT_INSTALL_EXAMPLES]/restclient/$$TARGET
!install_ok: INSTALLS += target
