TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += restclient

docTarget.target = doxygen
QMAKE_EXTRA_TARGETS += docTarget
