TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += restclient

docTarget.target = doxygen
docTarget.CONFIG += recursive
docTarget.recurse_target = doxygen
QMAKE_EXTRA_TARGETS += docTarget
