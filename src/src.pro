TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += restclient

restclient.CONFIG += no_lrelease_target

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease
