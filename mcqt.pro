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
        src/SettingsDialog.cpp

HEADERS  += src/MainWindow.h \
            src/PlayerListModel.h \
            src/SettingsDialog.h

FORMS    += forms/MainWindow.ui \
            forms/SettingsDialog.ui

DEFINES += PDC_WIDE \
    _WINSOCK_DEPRECATED_NO_WARNINGS \
    _CRT_SECURE_NO_WARNINGS \
    WIN32 \
    NDEBUG \
    _CONSOLE \
    _LIB

QMAKE_CXXFLAGS_RELEASE += /MD
QMAKE_CXXFLAGS_DEBUG += /MDd

INCLUDEPATH += $$PWD/lib/mclib
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/mclib/Release/ -lmclib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/mclib/Debug/ -lmclibd
else:unix: LIBS += -L$$PWD/lib/mclib/Release/ -lmclib

INCLUDEPATH += $$PWD/lib/jsoncpp/include
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/jsoncpp/lib/ -ljsoncpp-MT
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/jsoncpp/lib/ -ljsoncpp-MDd
else:unix: LIBS += -ljsoncpp
