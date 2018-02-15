TEMPLATE = subdirs

win32:cross_compile: CONFIG += winrt
!android:!ios:!winrt {
	SUBDIRS = qrestbuilder
	qrestbuilder.CONFIG = host_build
}

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease
