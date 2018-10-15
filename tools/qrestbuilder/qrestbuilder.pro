option(host_build)

QT = core
!force_bootstrap:qtHaveModule(xmlpatterns): QT += xmlpatterns

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
	xmlconverter.h \
	../../src/3rdparty/optional-lite/optional.hpp \
	../../src/3rdparty/variant-lite/variant.hpp

SOURCES += \
	main.cpp \
	restbuilder.cpp \
	objectbuilder.cpp \
	classbuilder.cpp \
	xmlconverter.cpp

XML_SCHEMA_DEFINITIONS += \
	qrestbuilder.xsd

contains(QT, xmlpatterns): RESOURCES += qrestbuilder.qrc

INCLUDEPATH +=  \
	../../src/3rdparty/optional-lite \
	../../src/3rdparty/variant-lite

load(qt_tool)

win32 {
	QMAKE_TARGET_PRODUCT = "Qt Rest API Builder"
	QMAKE_TARGET_COMPANY = $$COMPANY
	QMAKE_TARGET_COPYRIGHT = "Felix Barz"
} else:mac {
	QMAKE_TARGET_BUNDLE_PREFIX = $${BUNDLE_PREFIX}.
}

!ReleaseBuild:!DebugBuild:!system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)): error(qpmx initialization failed. Check the compilation log for details.)
else: include($$OUT_PWD/qpmx_generated.pri)
