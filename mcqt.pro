#-------------------------------------------------
#
# Project created by QtCreator 2016-01-31T23:41:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mcqt
TEMPLATE = app


SOURCES += src/main.cpp\
        src/MainWindow.cpp \
        src/PlayerListModel.cpp \
        src/SettingsDialog.cpp \
    src/Settings.cpp

HEADERS  += src/MainWindow.h \
            src/PlayerListModel.h \
            src/SettingsDialog.h \
    src/Settings.h

FORMS    += forms/MainWindow.ui \
            forms/SettingsDialog.ui

win32:DEFINES += PDC_WIDE \
    _WINSOCK_DEPRECATED_NO_WARNINGS \
    _CRT_SECURE_NO_WARNINGS \
    WIN32 \
    NDEBUG \
    _CONSOLE \
    _LIB

win32:QMAKE_CXXFLAGS_RELEASE += /MD
win32:QMAKE_CXXFLAGS_DEBUG += /MDd
unix:QMAKE_CXXFLAGS += -std=c++11

win32:RC_ICONS += icons/mcqt256.ico icons/mcqt128.ico icons/mcqt64.ico icons/mcqt32.ico icons/mcqt16.ico

INCLUDEPATH += $$PWD/lib/mclib/mclib/include
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/mclib/Release/ -lmclib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/mclib/Debug/ -lmclibd
else:unix: LIBS += -L$$PWD/lib/mclib/ -lmc

INCLUDEPATH += $$PWD/lib/mclib/lib/jsoncpp/include
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/mclib/lib/jsoncpp/lib/ -ljsoncpp-msvc-2017
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/mclib/lib/jsoncpp/lib/ -ljsoncppd
#else:unix: LIBS += -ljsoncpp
