TEMPLATE = subdirs

!no_json_serializer: SUBDIRS += qrestbuilder

qrestbuilder.CONFIG += no_lrelease_target

prepareRecursiveTarget(lrelease)
QMAKE_EXTRA_TARGETS += lrelease

QMAKE_EXTRA_TARGETS += run-tests
