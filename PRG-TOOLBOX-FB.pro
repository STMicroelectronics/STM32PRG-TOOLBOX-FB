TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
DESTDIR = $$PWD
QMAKE_LFLAGS +=-static -static-libgcc -static-libstdc++
LIBS += -lstdc++fs
MAKEFILE = qtMakefile

VERSION = 2.1.0
QMAKE_TARGET_COMPANY = "STMicroelectronics"
QMAKE_TARGET_PRODUCT = "PRG-TOOLBOX-FB"
QMAKE_TARGET_COPYRIGHT = "Copyrights 2024 STMicroelectronics"

INCLUDEPATH += $$PWD/Inc

SOURCES += \
        Src/DisplayManager.cpp \
        Src/FileManager.cpp \
        Src/ProgramManager.cpp \
        Src/Fastboot.cpp \
        Src/main.cpp

HEADERS += \
    Inc/DisplayManager.h \
    Inc/Error.h \
    Inc/FileManager.h \
    Inc/ProgramManager.h \
    Inc/main.h \
    Inc/Fastboot.h \

DISTFILES += \
    License.txt \
    README.txt
