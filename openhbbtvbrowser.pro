TEMPLATE = app
TARGET = openhbbtvbrowser
QT += webenginewidgets
CONFIG += c++11
CONFIG += console

cross_compile: DEFINES += EMBEDDED_BUILD

HEADERS += \
    browsercontrol.h \
    browserwindow.h \
    webpage.h \
    webview.h

SOURCES += \
    main.cpp \
    browsercontrol.cpp \
    browserwindow.cpp \
    webpage.cpp \
    webview.cpp

RESOURCES += openhbbtvbrowser.qrc
