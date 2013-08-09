QT       += core gui webkit network

TEMPLATE = app
CONFIG +=   console

SOURCES = WebBrowser.cpp \
    main.cpp \
    mynetworkaccessmanager.cpp

HEADERS = WebBrowser.h \
    mynetworkaccessmanager.h

win32:TARGET = ../Auc_4_8_0

OTHER_FILES += \
    ../../auc_last/HTMLsourse.html \
    ../../auc_last/htmlsourse2.txt \
    ../../auc_bin_release_4.8.0/temp.html \
    temp.txt
