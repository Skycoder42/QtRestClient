QT += core gui widgets restclient network

TARGET = SimpleRestClientApp
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
		mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

target.path = $$[QT_INSTALL_EXAMPLES]/restclient/SimpleRestClientApp
INSTALLS += target

#not found by linker?
LIBS += -L/home/sky/Qt/5.8/gcc_64/lib/ -licudata
LIBS += -L/home/sky/Qt/5.8/gcc_64/lib/ -licui18n
LIBS += -L/home/sky/Qt/5.8/gcc_64/lib/ -licuuc
