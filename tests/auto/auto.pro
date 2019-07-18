TEMPLATE = subdirs

SUBDIRS += cmake \
	restclient \
	qml

qml.depends += restclient

!wasm {
	SUBDIRS += restclientauth
	restclientauth.depends += restclient
}

cmake.CONFIG += no_run-tests_target
prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
