TEMPLATE = subdirs

android: CONFIG += no_coroutine_tests
contains(QMAKE_COMPILER_DEFINES, _MSC_VER=1900): CONFIG += no_coroutine_tests

SUBDIRS += \
	PagingModelTest \
	testlib \
	RequestBuilderTest \
	RestClientTest \
	RestReplyTest \
	IntegrationTest \
	RestBuilderTest \

!no_coroutine_tests: SUBDIRS += RestAwaitablesTest

RequestBuilderTest.depends += testlib
RestClientTest.depends += testlib
RestReplyTest.depends += testlib
IntegrationTest.depends += testlib
RestBuilderTest.depends += testlib
RestAwaitablesTest.depends += testlib
PagingModelTest.depends += testlib

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
