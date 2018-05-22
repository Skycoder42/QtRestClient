option(host_build)

QT = core

TARGET = qrestbuilder
VERSION = $$MODULE_VERSION
COMPANY = Skycoder42
BUNDLE_PREFIX = de.skycoder42

DEFINES += BUILD_QRESTBUILDER
DEFINES += "TARGET=\\\"$$TARGET\\\""
DEFINES += "VERSION=\\\"$$VERSION\\\""
DEFINES += "COMPANY=\\\"$$COMPANY\\\""
DEFINES += "BUNDLE_PREFIX=\\\"$$BUNDLE_PREFIX\\\""

HEADERS += \
	restbuilder.h \
	objectbuilder.h \
	classbuilder.h \
	xmlconverter.h

SOURCES += \
	main.cpp \
	restbuilder.cpp \
	objectbuilder.cpp \
	classbuilder.cpp \
	xmlconverter.cpp

load(qt_tool)

win32 {
	QMAKE_TARGET_PRODUCT = "Qt Rest API Builder"
	QMAKE_TARGET_COMPANY = $$COMPANY
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = $${BUNDLE_PREFIX}.
}

DISTFILES += qrestbuilder.xsd
