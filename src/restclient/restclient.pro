TARGET = QtRestClient

QT = core network jsonserializer

OTHER_FILES += doc/Doxyfile
OTHER_FILES += doc/makedoc.sh
OTHER_FILES += doc/*.dox
OTHER_FILES += doc/snippets/*.cpp

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
	standardpaging.h \
	metacomponent.h

PRIVATE_HEADERS += \
	restclass_p.h \
	restclient_p.h \
	restreply_p.h

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES += \
	requestbuilder.cpp \
	restclass.cpp \
	restclient.cpp \
	restreply.cpp \
	standardpaging.cpp

load(qt_module)

win32 {
	QMAKE_TARGET_PRODUCT = "QtRestClient"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}

docTarget.target = doxygen
docTarget.commands = chmod u+x $$PWD/doc/makedoc.sh && $$PWD/doc/makedoc.sh "$$PWD" "$$VERSION" "$$[QT_INSTALL_BINS]" "$$[QT_INSTALL_HEADERS]" "$$[QT_INSTALL_DOCS]"
QMAKE_EXTRA_TARGETS += docTarget
