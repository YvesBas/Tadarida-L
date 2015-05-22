#-------------------------------------------------
#
# Project created by QtCreator 2013-01-08T11:13:07
#
#-------------------------------------------------

QT       += core gui widgets


TARGET = TadaridaQt
TEMPLATE = app

win32 {
DEFINES += BUILDTIME=\\\"$$system('echo %time%')\\\"
DEFINES += BUILDDATE=\\\"$$system('echo %date%')\\\"
} else {
DEFINES += BUILDTIME=\\\"$$system(date '+%H:%M.%s')\\\"
DEFINES += BUILDDATE=\\\"$$system(date '+%d/%m/%y')\\\"
}

SOURCES += main.cpp\
    fenim.cpp \
    etiquette.cpp \
    param.cpp \
    loupe.cpp \
    detec.cpp \
    TadaridaMainWindow.cpp \
    recherche.cpp \
    detectreatment.cpp

HEADERS  += \
    fenim.h \
    etiquette.h \
    param.h \
    loupe.h \
    detec.h \
    TadaridaMainWindow.h \
    recherche.h \
    detectreatment.h

INCLUDEPATH += "C:/Program Files (x86)/Mega-Nerd/libsndfile/include" "Headers" 

win32: LIBS += -L$$PWD/Libs/ -llibfftw3f-3 -llibsndfile-1

