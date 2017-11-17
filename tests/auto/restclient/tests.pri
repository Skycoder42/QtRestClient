QT += core network jsonserializer restclient

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../testlib/release/ -ltestlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../testlib/debug/ -ltestlib
else:unix: LIBS += -L$$OUT_PWD/../testlib/ -ltestlib

INCLUDEPATH += $$PWD/testlib
DEPENDPATH += $$PWD/testlib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../testlib/release/libtestlib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../testlib/debug/libtestlib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../testlib/release/testlib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../testlib/debug/testlib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../testlib/libtestlib.a

mac: QMAKE_LFLAGS += '-Wl,-rpath,\'$$OUT_PWD/../../../../lib\''

DEFINES -= QT_ASCII_CAST_WARNINGS
