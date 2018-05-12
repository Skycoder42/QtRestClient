TEMPLATE = subdirs

SUBDIRS += restclient \
	imports

restclient.CONFIG += no_lrelease_target
imports.depends += restclient
imports.CONFIG += no_lrelease_target

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease
