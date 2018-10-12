TEMPLATE = subdirs

SUBDIRS += \
	testlib \
	RequestBuilderTest \
	RestClientTest \
	RestReplyTest \
	IntegrationTest \
	RestBuilderTest

RequestBuilderTest.depends += testlib
RestClientTest.depends += testlib
RestReplyTest.depends += testlib
IntegrationTest.depends += testlib
RestBuilderTest.depends += testlib

prepareRecursiveTarget(run-tests)
QMAKE_EXTRA_TARGETS += run-tests
