QT       += core gui widgets restclient

TARGET = JsonPlaceholderApi
TEMPLATE = app

SOURCES += main.cpp\
		mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

REST_API_OBJECTS += post.json
REST_API_CLASSES += api.json \
	api_posts.json

target.path = $$[QT_INSTALL_EXAMPLES]/restclient/$$TARGET
INSTALLS += target

#add lib dir to rpath
mac: QMAKE_LFLAGS += '-Wl,-rpath,\'$$OUT_PWD/../../../lib\''
