TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        src/ConditionFlags.cpp \
        src/State8080.cpp \
        src/main.cpp

HEADERS += \
    include/ConditionFlags.hpp \
    include/State8080.hpp

INCLUDEPATH += \
    include/

RESOURCES += \
    resources.qrc
