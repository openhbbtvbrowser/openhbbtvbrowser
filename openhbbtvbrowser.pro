TEMPLATE = app
TARGET = openhbbtvbrowser
QT += webenginewidgets
CONFIG += c++11

cross_compile: DEFINES += EMBEDDED_BUILD

HEADERS += \
    browsercontrol.h \
    browserwindow.h \
    webview.h

SOURCES += \
    main.cpp \
    browsercontrol.cpp \
    browserwindow.cpp \
    webview.cpp

RESOURCES += openhbbtvbrowser.qrc
