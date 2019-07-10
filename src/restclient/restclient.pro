TARGET = QtRestClient

QT = core network
MODULE_CONFIG += qrestbuilder

!no_json_serializer {
	!qtHaveModule(jsonserializer): warning("Unable to find QtJsonSerializer module. To build without it, add \"CONFIG+=no_json_serializer\" to your qmake command line")
	QT += jsonserializer
} else {
	MODULE_DEFINES += Q_RESTCLIENT_NO_JSON_SERIALIZER
	DEFINES += Q_RESTCLIENT_NO_JSON_SERIALIZER
}

HEADERS += \
	restclass_p.h \
	restclient_p.h \
	restreply_p.h \
	qtrestclient_global.h \
	ipaging.h \
	requestbuilder.h \
	restclass.h \
	restclient.h \
	restreply.h \
	standardpaging_p.h \
	restreplyawaitable.h \
	restreplyawaitable_p.h

!no_json_serializer {
	HEADERS += \
		metacomponent.h \
		paging_fwd.h \
		paging.h \
		genericrestreply.h \
		simple.h
}

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
