TEMPLATE = app
CONFIG +=  c++11
#CONFIG -= app_bundle

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


SOURCES += \
        src/mainwindow.cpp \
        src/ConditionFlags.cpp \
        src/Disassembler8080.cpp \
        src/State8080.cpp \
        src/main.cpp \
        test/cpudiagtest.cpp \
        test/manualtest.cpp \
        test/optest.cpp

HEADERS += \
    include/ConditionFlags.hpp \
    include/Disassembler8080.hpp \
    include/State8080.hpp \
    include/mainwindow.hpp \
    include/tester.h

INCLUDEPATH += \
    include/

RESOURCES += \
    resources.qrc

FORMS += \
    mainwindow.ui


# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target
