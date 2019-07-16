TEMPLATE = subdirs

SUBDIRS += restclient \
	imports \
	restclientauth

restclient.CONFIG += no_lrelease_target
restclientauth.depends += restclient
imports.depends += restclient restclientauth
imports.CONFIG += no_lrelease_target

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease

QMAKE_EXTRA_TARGETS += run-tests
