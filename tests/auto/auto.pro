TEMPLATE = subdirs

SUBDIRS += cmake \
	restclient \
	qml \
	restclientauth

restclientauth.depends += restclient
qml.depends += restclient

cmake.CONFIG += no_run-tests_target
prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
