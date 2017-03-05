TEMPLATE = subdirs
SUBDIRS = qrestbuilder

qrestbuilder.CONFIG = host_build

docTarget.target = doxygen
QMAKE_EXTRA_TARGETS += docTarget
