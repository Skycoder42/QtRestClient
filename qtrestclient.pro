load(qt_parts)

SUBDIRS += doc

doxygen.target = doxygen
doxygen.CONFIG = recursive
doxygen.recurse_target = doxygen
doxygen.recurse += doc
QMAKE_EXTRA_TARGETS += doxygen

lrelease.target = lrelease
lrelease.CONFIG = recursive
lrelease.recurse_target = lrelease
lrelease.recurse = sub_src sub_tools
QMAKE_EXTRA_TARGETS += lrelease

DISTFILES += .qmake.conf \
	sync.profile

static_host_build: SUBDIRS = tools
