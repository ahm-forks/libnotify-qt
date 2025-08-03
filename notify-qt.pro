# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

TARGET = notify-qt
TEMPLATE = lib
CONFIG += qt staticlib create_prl

QT = core gui dbus

HEADERS = \
   $$PWD/src/include/libnotify-qt.h \
   $$PWD/src/Interface.h \
   $$PWD/src/libnotify-qt_p.h \
   $$PWD/src/logging.h

SOURCES = \
   $$PWD/src/Event.cpp \
   $$PWD/src/Interface.cpp \
   $$PWD/src/Manager.cpp

INCLUDEPATH = \
    $$PWD/src \
    $$PWD/src/include
