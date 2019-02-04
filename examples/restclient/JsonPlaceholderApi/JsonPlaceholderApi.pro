TEMPLATE = app

QT += core gui widgets restclient

TARGET = JsonPlaceholderApi

SOURCES += main.cpp \
	mainwindow.cpp

HEADERS += mainwindow.h

FORMS += mainwindow.ui

REST_API_FILES += post.xml \
	api.xml \
	api_posts.xml

target.path = $$[QT_INSTALL_EXAMPLES]/restclient/$$TARGET
INSTALLS += target
