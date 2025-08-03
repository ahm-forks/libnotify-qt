win32 | macx {
    error(Unsupported platform: win32!)
}

QT += gui core dbus
DEPENDPATH += $$PWD/.
INCLUDEPATH += $$PWD/src/include
LIBS += -L$$OUT_PWD/notify-qt -lnotify-qt
HEADERS += $$PWD/src/include/libnotify-qt.h

QMAKE_CLEAN += $$OUT_PWD/notify-qt/*
notify-qt.target = $$OUT_PWD/notify-qt/libnotify-qt.a
notify-qt.commands = $(MAKE) -C $$OUT_PWD/notify-qt -f $(MAKEFILE)
notify-qt.depends = \
    notify-qt-make \
    $$PWD/src/include/libnotify-qt.h \
    $$files($$PWD/src/*.h) \
    $$files($$PWD/src/*.cpp) \

notify-qt-make.target = $$OUT_PWD/notify-qt/$(MAKEFILE)
notify-qt-make.commands = mkdir -p $$OUT_PWD/notify-qt
notify-qt-make.commands += && $(QMAKE) -o $$OUT_PWD/notify-qt/$(MAKEFILE) $$PWD/notify-qt.pro
notify-qt-make.depends = $$PWD/notify-qt.pro

QMAKE_EXTRA_TARGETS += notify-qt notify-qt-make

PRE_TARGETDEPS += $$OUT_PWD/notify-qt/libnotify-qt.a
