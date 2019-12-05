TEMPLATE = subdirs

SUBDIRS += \
#	TestQmlRestClient

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
