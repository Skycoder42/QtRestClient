TARGET = QtRestClient

QT = core network jsonserializer

PUBLIC_HEADERS += \
	qrestclient_global.h \
	genericrestreply.h \
	ipaging.h \
	paging_fwd.h \
	paging.h \
	requestbuilder.h \
	restclass_p.h \
	restclass.h \
	restclient_p.h \
	restclient.h \
	restexception.h \
	restreply_p.h \
	restreply.h \
	simple.h \
	standardpaging.h

PRIVATE_HEADERS +=

SOURCES += \
	requestbuilder.cpp \
	restclass.cpp \
	restclient.cpp \
	restexception.cpp \
	restreply.cpp \
	standardpaging.cpp

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

load(qt_module)
