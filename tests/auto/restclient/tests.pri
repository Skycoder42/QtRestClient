QT += core network restclient httpserver

isEmpty(LIB_PWD): LIB_PWD = $$OUT_PWD/../testlib

win32:!win32-g++:CONFIG(release, debug|release): LIBS += -L$$LIB_PWD/release/ -ltestlib
else:win32:!win32-g++:CONFIG(debug, debug|release): LIBS += -L$$LIB_PWD/debug/ -ltestlib
else: LIBS += -L$$LIB_PWD/ -ltestlib

INCLUDEPATH += $$PWD/testlib
DEPENDPATH += $$PWD/testlib

win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$LIB_PWD/release/testlib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$LIB_PWD/debug/testlib.lib
else: PRE_TARGETDEPS += $$LIB_PWD/libtestlib.a

DEFINES -= QT_ASCII_CAST_WARNINGS

include($$PWD/../testrun.pri)
