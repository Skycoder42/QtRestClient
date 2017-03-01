TEMPLATE = subdirs

SUBDIRS = restclient

docTarget.target = doxygen
QMAKE_EXTRA_TARGETS += docTarget