TARGET = QtRestClientAuth

QT = core network networkauth restclient restclient-private

HEADERS = \
	authrequestbuilder.h \
	authrequestbuilder_p.h \
	authrestclient.h \
	authrestclient_p.h \
	qtrestclientauth_global.h

SOURCES = \
	authrequestbuilder.cpp \
	authrestclient.cpp

load(qt_module)

win32 {
	QMAKE_TARGET_PRODUCT = "$$TARGET"
	QMAKE_TARGET_COMPANY = "Skycoder42"
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = "de.skycoder42."
}
