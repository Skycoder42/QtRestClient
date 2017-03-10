option(host_build)

QT = core
CONFIG += console c++11

TARGET = qrestbuilder
VERSION = $$MODULE_VERSION
COMPANY = Skycoder42
BUNDLE_PREFIX = de.skycoder42

DEFINES += BUILD_QSCXMLC
DEFINES += "TARGET=\\\"$$TARGET\\\""
DEFINES += "VERSION=\\\"$$VERSION\\\""
DEFINES += "COMPANY=\\\"$$COMPANY\\\""
DEFINES += "DOMAIN=\\\"$$BUNDLE_PREFIX\\\""

load(qt_tool)
load(resources)

SOURCES += \
	main.cpp \
	restbuilder.cpp \
	objectbuilder.cpp \
	classbuilder.cpp

HEADERS += \
	restbuilder.h \
	objectbuilder.h \
	classbuilder.h

RESOURCES += \
	qrestbuilder.qrc

win32 {
	QMAKE_TARGET_PRODUCT = "Qt Rest API Builder"
	QMAKE_TARGET_COMPANY = $$COMPANY
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = $${BUNDLE_PREFIX}.
}

#not found by linker?
unix:!mac {
	LIBS += -L$$[QT_INSTALL_LIBS] -licudata
	LIBS += -L$$[QT_INSTALL_LIBS] -licui18n
	LIBS += -L$$[QT_INSTALL_LIBS] -licuuc
}

feature.path = $$[QT_INSTALL_DATA]/mkspecs/features
feature.files = $$PWD/../../mkspecs/features/qrestbuilder.prf
INSTALLS += feature
