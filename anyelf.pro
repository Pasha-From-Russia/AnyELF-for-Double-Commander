CONFIG -= qt

TEMPLATE = lib

DISTFILES += \
    Makefile

HEADERS += \
    anyelf.h \
    common.h

SOURCES += \
    anyelf_common.cpp \
    anyelf_gtk.cpp \
    anyelf_qt5.cpp \
    anyelfdump.cpp
