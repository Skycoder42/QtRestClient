TEMPLATE = lib

QT       += network
QT       -= gui

TARGET = QtRestClient
VERSION = 0.2.0

win32 {
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_PRODUCT = "QtRestClient"
	QMAKE_TARGET_DESCRIPTION = $$QMAKE_TARGET_PRODUCT
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"

	CONFIG += skip_target_version_ext
	LIBS += -lUser32
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.Skycoder42."
	QMAKE_FRAMEWORK_BUNDLE_NAME = "QtRestClient"

	CONFIG += lib_bundle
	QMAKE_LFLAGS_SONAME = '-Wl,-install_name,@rpath/'
	QMAKE_LFLAGS += '-Wl,-rpath,\'@executable_path/../Frameworks\''
}

CONFIG += qjs_as_lib
include(../QJsonSerializer/qjsonserializer.pri)

DEFINES += QTRESTCLIENT_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
	restclient.cpp \
	requestbuilder.cpp \
	restclass.cpp \
	restreply.cpp \
	restobject.cpp \
	restexception.cpp \
	standardpaging.cpp

HEADERS +=\
		qtrestclient_global.h \
	restclient.h \
	restclient_p.h \
	QtRestClient \
	requestbuilder.h \
	restclass.h \
	restclass_p.h \
	restreply.h \
	restreply_p.h \
	restobject.h \
	restexception.h \
	genericrestreply.h \
	ipaging.h \
	standardpaging.h \
	paging.h \
	paging_fwd.h \
	simple.h

unix {
	target.path = /usr/lib
	INSTALLS += target
}
