TEMPLATE = subdirs

SUBDIRS = qrestbuilder

no_host_tools:android|ios|winrt {
	SUBDIRS -= qrestbuilder
}

qrestbuilder.CONFIG += no_lrelease_target

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease
