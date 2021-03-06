#-------------------------------------------------
#
# Project created by QtCreator 2019-12-06T13:08:56
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CbsdMixer
TEMPLATE = app

CONFIG += qwt
#INCLUDEPATH += /usr/include/qwt
#LIBS += -l qwt-qt5

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        plotwindow.cpp \
    mixer.cpp \
    audioio.cpp \
	Filters/filter.cpp \
    Filters/rectangularfilter.cpp \
    Filters/triangularfilter.cpp \
    Filters/cosinefilter.cpp \
    filterwindow.cpp

HEADERS += \
        plotwindow.h \
    mixer.h \
    audioio.h \
	Filters/filter.h \
    Filters/rectangularfilter.h \
    Filters/triangularfilter.h \
    Filters/cosinefilter.h \
    Filters/filter_types.h \
    filterwindow.h

FORMS += \
        plotwindow.ui \
    filterwindow.ui

QMAKE_CXXFLAGS += -lasound -lfftw3
LIBS += -lasound -lfftw3

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
