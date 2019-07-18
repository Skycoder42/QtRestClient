TEMPLATE = subdirs

SUBDIRS += restclient \
	imports

imports.depends += restclient

!wasm {
	SUBDIRS += restclientauth
	restclientauth.depends += restclient
	imports.depends += restclientauth
}

QMAKE_EXTRA_TARGETS += run-tests
