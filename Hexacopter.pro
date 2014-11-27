#-------------------------------------------------
#
# Project created by QtCreator 2014-11-12T09:48:07
#
#-------------------------------------------------

QT       += core gui opengl widgets serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Hexacopter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        copter.cpp \
        glwidget.cpp \
        matlib.cpp \
    gateway-linux/src/independent/checksumes.cpp \
    gateway-linux/src/independent/gateway.cpp \
    gateway-linux/src/independent/linkinterfacecan.cpp \
    gateway-linux/src/independent/linkinterfaceuart.cpp \
    gateway-linux/src/independent/linkinterfaceudp.cpp \
    gateway-linux/src/independent/stream-bytesex.cpp \
    gateway-linux/src/independent/udp.cpp \
    gateway-linux/src/linux/hal/hal_can.cpp \
    gateway-linux/src/linux/hal/hal_uart.cpp \
    gateway-linux/src/linux/hw_udp.cpp \
    qcustomplot/qcustomplot.cpp

HEADERS += mainwindow.h \
        copter.h \
        glwidget.h \
        datatypes.h \
        matlib.h \
    gateway-linux/api/gateway/gateway.h \
    gateway-linux/api/gateway/linkinterface.h \
    gateway-linux/api/gateway/linkinterfacecan.h \
    gateway-linux/api/gateway/linkinterfaceuart.h \
    gateway-linux/api/gateway/linkinterfaceudp.h \
    gateway-linux/api/gateway/networkmessage.h \
    gateway-linux/api/hal/genericIO.h \
    gateway-linux/api/hal/hal_can.h \
    gateway-linux/api/hal/hal_uart.h \
    gateway-linux/api/checksumes.h \
    gateway-linux/api/fifo.h \
    gateway-linux/api/gateway.h \
    gateway-linux/api/hal.h \
    gateway-linux/api/putter.h \
    gateway-linux/api/stream-bytesex.h \
    gateway-linux/api/udp.h \
    gateway-linux/src/linux/hw_udp.h \
    qcustomplot/qcustomplot.h

FORMS   += mainwindow.ui


#VPATH += ../shared
#INCLUDEPATH += ../shared

#HEADERS       = glwidget.h \
#                window.h \
#                qtlogo.h
#SOURCES       = glwidget.cpp \
#                main.cpp \
#                window.cpp \
#                qtlogo.cpp
#QT           += opengl widgets serialport

## install
#target.path = $$[QT_INSTALL_EXAMPLES]/opengl/hellogl
#INSTALLS += target

#contains(QT_CONFIG, opengles.) {
#    contains(QT_CONFIG, angle): \
#        warning("Qt was built with ANGLE, which provides only OpenGL ES 2.0 on top of DirectX 9.0c")
#    error("This example requires Qt to be configured with -opengl desktop")
#}



#unix:!macx: LIBS += -L$$PWD/../git/rodos-core/libs/linux_x86/ -lrodos

#INCLUDEPATH += $$PWD/../git/rodos-core/libs/linux_x86
#DEPENDPATH += $$PWD/../git/rodos-core/libs/linux_x86

#unix:!macx: PRE_TARGETDEPS += $$PWD/../git/rodos-core/libs/linux_x86/librodos.a
