TARGET = QtRestClient

QT = core network jsonserializer

PUBLIC_HEADERS += \
	qrestclient_global.h \
	genericrestreply.h \
	ipaging.h \
	paging_fwd.h \
	paging.h \
	requestbuilder.h \
	restclass.h \
	restclient.h \
	restreply.h \
	simple.h \
	standardpaging.h

PRIVATE_HEADERS += \
	restclass_p.h \
	restclient_p.h \
	restreply_p.h

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS \
    metacomponent.h

SOURCES += \
	requestbuilder.cpp \
	restclass.cpp \
	restclient.cpp \
	restreply.cpp \
	standardpaging.cpp

load(qt_module)

win32 {
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_PRODUCT = "QtRestClient"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}
