TARGET = QtRestClient

QT = core network jsonserializer
MODULE_CONFIG += c++14 qrestbuilder

HEADERS +=  \
	restclass_p.h \
	restclient_p.h \
	restreply_p.h \
	qtrestclient_global.h \
	genericrestreply.h \
	ipaging.h \
	paging_fwd.h \
	paging.h \
	requestbuilder.h \
	restclass.h \
	restclient.h \
	restreply.h \
	simple.h \
	metacomponent.h \
	standardpaging_p.h \
	restreplyawaitable.h \
	restreplyawaitable_p.h

SOURCES += \
	requestbuilder.cpp \
	restclass.cpp \
	restclient.cpp \
	restreply.cpp \
	standardpaging.cpp \
	ipaging.cpp \
	restreplyawaitable.cpp

load(qt_module)

FEATURES += ../../mkspecs/features/qrestbuilder.prf
features.files = $$FEATURES
features.path = $$[QT_HOST_DATA]/mkspecs/features/
INSTALLS += features

lib_bundle: FRAMEWORK_HEADERS.files += \
	$$absolute_path(Paging, $$INC_PATH/include/$$MODULE_INCNAME) \
	$$absolute_path(RestReply, $$INC_PATH/include/$$MODULE_INCNAME)
else: gen_headers.files += \
	$$absolute_path(Paging, $$INC_PATH/include/$$MODULE_INCNAME) \
	$$absolute_path(RestReply, $$INC_PATH/include/$$MODULE_INCNAME)

win32 {
	QMAKE_TARGET_PRODUCT = "QtRestClient"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}
