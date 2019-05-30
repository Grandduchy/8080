TEMPLATE = app
CONFIG += c++11
#CONFIG -= app_bundle

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


SOURCES += \
        src/debugwindow.cpp \
        src/mainwindow.cpp \
        src/ConditionFlags.cpp \
        src/Disassembler8080.cpp \
        src/State8080.cpp \
        src/main.cpp \
        test/manualtest.cpp \
        test/tests.cpp

HEADERS += \
    include/ConditionFlags.hpp \
    include/Disassembler8080.hpp \
    include/State8080.hpp \
    include/mainwindow.hpp \
    include/tester.h \
    include/debugwindow.hpp

INCLUDEPATH += \
    include/

RESOURCES += \
    resources.qrc

FORMS += \
    forms/mainwindow.ui \
    forms/debugwindow.ui


# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    test/allowRunTests.py
