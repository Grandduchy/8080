TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../src/ConditionFlags.cpp \
    ../src/Disassembler8080.cpp \
    ../src/State8080.cpp \
    tests.cpp

HEADERS += \
    ../include/ConditionFlags.hpp \
    ../include/Disassembler8080.hpp \
    ../include/State8080.hpp \
    ../include/tester.h

INCLUDEPATH += \
    ../include

DISTFILES += \
    allowRunTests.py
