QT += network qml jsonserializer

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../QtRestClient/release/ -lQt5RestClient
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../QtRestClient/debug/ -lQt5RestClient
else:mac: LIBS += -F$$OUT_PWD/../../QtRestClient/ -framework lQt5RestClient
else:unix: LIBS += -L$$OUT_PWD/../../QtRestClient/ -lQt5RestClient

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
