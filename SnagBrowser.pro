#-------------------------------------------------
#
# Project created by QtCreator 2016-12-31T13:36:21
#
#-------------------------------------------------

QT       += core gui webkit webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SnagBrowser
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

LIBS    +=  -lSDL2
