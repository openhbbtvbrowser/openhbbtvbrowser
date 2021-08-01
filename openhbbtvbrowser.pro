TEMPLATE = app
TARGET = openhbbtvbrowser
QT += webenginewidgets
CONFIG += c++1z
CONFIG += console

cross_compile: DEFINES += EMBEDDED_BUILD

HEADERS += \
    virtualkey.h \
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
