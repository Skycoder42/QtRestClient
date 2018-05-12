TEMPLATE = subdirs

SUBDIRS += cmake \
	restclient \
	qml

qml.depends += restclient
