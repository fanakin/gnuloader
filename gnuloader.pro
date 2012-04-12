TEMPLATE = app
TARGET = ./gnuloader/gnuloader

CONFIG += qt opengl thread release link_prl
QT += qt3support opengl xml gui

win32:CONFIG += windows
win32:DESTDIR = ..\win32
debug:CONFIG += console


OBJECTS_DIR = ./.obj
MOC_DIR = ./.moc

HEADERS += ./gnuloader/config.h
HEADERS += ./gnuloader/main.h
HEADERS += ./gnuloader/CAppoggio.h
HEADERS += ./gnuloader/commfunc.h
HEADERS += ./gnuloader/Containers.h
HEADERS += ./gnuloader/realtemplateclasses.h
HEADERS += ./gnuloader/TabStrc.h
HEADERS += ./gnuloader/WTDDLibreria.h
HEADERS += ./gnuloader/command018um.h

SOURCES += ./gnuloader/main.cpp
SOURCES += ./gnuloader/CAppoggio.cpp
SOURCES += ./gnuloader/commfunc.cpp
SOURCES += ./gnuloader/Containers.cpp
SOURCES += ./gnuloader/TabStrc.cpp
SOURCES += ./gnuloader/WTDDLibreria.cpp
SOURCES += ./gnuloader/command018um.cpp

INCLUDEPATH = . ./gnuloader ../../../library

# if we want idps, uncomment the following line:
#
#CONFIG += idps
#

LIBS = -L ../../../library
win32:release { LIBS += -L ..\library\release }
win32:debug { LIBS += -L ..\library\debug }

win32:LIBS += -lws2_32 -lopengl32 -lglu32

unix:LIBS += -L ../../../library -larn

QMAKE_CXXFLAGS_DEBUG += -g -O0
QMAKE_CXXFLAGS_RELEASE += -O2


#unix:idps {
# LIBS += -lidps-3.2.3-qt4
# DEFINES += HAVE_IDPS
#}



