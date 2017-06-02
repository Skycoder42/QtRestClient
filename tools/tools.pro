TEMPLATE = subdirs

win32:cross_compile: CONFIG += winrt
!android:!ios:!winrt {
	SUBDIRS = qrestbuilder
	qrestbuilder.CONFIG = host_build
}

feature.path = $$[QT_INSTALL_ARCHDATA]/mkspecs/features
feature.files = $$PWD/../mkspecs/features/qrestbuilder.prf
INSTALLS += feature

docTarget.target = doxygen
QMAKE_EXTRA_TARGETS += docTarget
