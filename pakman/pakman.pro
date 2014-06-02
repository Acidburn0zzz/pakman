#-------------------------------------------------
#
# Project created by QtCreator 2014-05-04T10:44:38
#
#-------------------------------------------------

QT       += core gui network xml

LIBS     += -lkdeui -lqjson

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG(release) {
  TARGET   = pakman
}
CONFIG(debug, debug_and_release): TARGET = pakmanD
TEMPLATE = app

CONFIG += qt warn_on c++11
# http://qt-project.org/doc/qt-4.8/qmake-variable-reference.html

equals(PWD, $${OUT_PWD}) {
  CONFIG(release) {
    DESTDIR = ../build/release
  }
  CONFIG(debug, debug_and_release) {
    DESTDIR = ../build/debug
  }
  OBJECTS_DIR = $$DESTDIR/.obj
  MOC_DIR = $$DESTDIR/.moc
  RCC_DIR = $$DESTDIR/.qrc
  UI_DIR = $$DESTDIR/.ui
}

#QMAKE_CXXFLAGS_RELEASE -= -O2 -march=x86-64
#QMAKE_CXXFLAGS_RELEASE += -O3 -march=native
#
QMAKE_CXXFLAGS         += -std=c++11
QMAKE_CXXFLAGS_RELEASE += -flto

QMAKE_LFLAGS           += -std=c++11
QMAKE_LFLAGS_RELEASE   += -flto


SOURCES += src/main.cpp \
           src/ui/groupbox.cpp \
           src/ui/infotabs.cpp \
           src/ui/lineedit.cpp \
           src/ui/mainwindow.cpp \
           src/ui/packageview.cpp \
           src/ui/statusbar.cpp \
           src/ui/whatprovidesme.cpp \
           src/commands/curlcommands.cpp \
           src/commands/pacman.cpp \
           src/commands/pacmancommands.cpp \
           src/commands/pacmanlogviewer.cpp \
           src/commands/taskprocessor.cpp \
           src/commands/terminal.cpp \
           src/data/packagerepository.cpp \
           src/data/distribution/distributioninfo.cpp \
           src/data/distribution/archlinuxinfo.cpp \
           src/data/model/defaultpackagefilter.cpp \
           src/data/model/packageitem.cpp \
           src/data/model/packagemodel.cpp \
           external/qt-solutions/qtsingleapplication.cpp \
           external/qt-solutions/qtlocalpeer.cpp \
           external/qt-solutions/qtlockedfile.cpp \
           external/qt-solutions/qtlockedfile_win.cpp \
           external/qt-solutions/qtlockedfile_unix.cpp \
           src/strconstants.cpp

HEADERS += src/ui/groupbox.h \
           src/ui/infotabs.h \
           src/ui/lineedit.h \
           src/ui/mainwindow.h \
           src/ui/packageview.h \
           src/ui/statusbar.h \
           src/ui/whatprovidesme.h \
           src/commands/curlcommands.h \
           src/commands/pacman.h \
           src/commands/pacmancommands.h \
           src/commands/pacmanlogviewer.h \
           src/commands/taskprocessor.h \
           src/commands/terminal.h \
           src/data/packagedata.h \
           src/data/packagerepository.h \
           src/data/distribution/distributioninfo.h \
           src/data/distribution/archlinuxinfo.h \
           src/data/model/defaultpackagefilter.h \
           src/data/model/packagefilter.h \
           src/data/model/packageitem.h \
           src/data/model/packagemodel.h \
           external/qt-solutions/QtSingleApplication \
           external/qt-solutions/QtLockedFile \
           external/qt-solutions/qtsingleapplication.h \
           external/qt-solutions/qtlocalpeer.h \
           external/qt-solutions/qtlockedfile.h \
           src/strconstants.h \
           src/icons.h

FORMS   += ui/groupbox.ui \
           ui/infotabs.ui \
           ui/mainwindow.ui \
           ui/packageview.ui \
           ui/whatprovidesme.ui

RESOURCES += \
           res/pakman-resource.qrc

OTHER_FILES += \
           ../DEPENDENCIES

TRANSLATIONS += \
           pakman_de.ts
