#-------------------------------------------------
#
# Project created by QtCreator 2018-09-02T18:24:56
#
#-------------------------------------------------

QT       += core gui
CONFIG   += C++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Insaniquarium
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    iobject.cpp \
    iscene.cpp \
    maindialog.cpp \
    icreature.cpp \
    ifish.cpp \
    ifood.cpp \
    imoney.cpp \
    ialien.cpp \
    ipet.cpp \
    igamedata.cpp \
    dlginputname.cpp \
    ibubble.cpp \
    button.cpp

HEADERS  += \
    iobject.h \
    iscene.h \
    maindialog.h \
    icreature.h \
    ifish.h \
    ifood.h \
    imoney.h \
    ialien.h \
    ipet.h \
    igamedata.h \
    dlginputname.h \
    ibubble.h \
    button.h

FORMS    += mainwindow.ui \
    maindialog.ui \
    quitwarningdialog.ui \
    dlginputname.ui

RESOURCES += \
    resource.qrc

TRANSLATIONS+=en.ts \
               ch.ts
