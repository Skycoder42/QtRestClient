QT += network qml

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../QtRestClient/release/ -lQtRestClient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../QtRestClient/debug/ -lQtRestClient
else:mac: LIBS += -F$$OUT_PWD/../../QtRestClient/ -framework QtRestClient
else:unix: LIBS += -L$$OUT_PWD/../../QtRestClient/ -lQtRestClient

INCLUDEPATH += $$PWD/../QtRestClient
DEPENDPATH += $$PWD/../QtRestClient

HEADERS += \
    $$PWD/tst_global.h \
    $$PWD/jphpost.h

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/jphpost.cpp \
    $$PWD/tst_global.cpp

DISTFILES += \
    $$PWD/rest-db-setup.bat \
    $$PWD/default-test-db.js \
    $$PWD/advanced-test-db.js

DEFINES += "TEST_SRC_DIR=\\\"$$PWD\\\""
