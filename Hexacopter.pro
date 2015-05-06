#-------------------------------------------------
#
# Project created by QtCreator 2014-11-12T09:48:07
#
#-------------------------------------------------

QT       += core gui opengl widgets serialport printsupport webkit webkitwidgets

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
    qcustomplot/qcustomplot.cpp \
    myitem.cpp \
    opticaltracking.cpp \
    gpscontrol/gpscoordinate.cpp \
    gpscontrol/gpstracking.cpp

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
    qcustomplot/qcustomplot.h \
    gateway-linux/api/gateway/topiclistreport.h \
    topics.h \
    myitem.h \
    opticaltracking.h \
    gpscontrol/gpscoordinate.h \
    gpscontrol/gpstracking.h

FORMS   += mainwindow.ui

LIBS += -L/usr/local/lib -lvrpn -lquat -pthread

RESOURCES += gpscontrol/gps.qrc \
            img/img.qrc

OTHER_FILES += gpscontrol/gTracking.html \
               gpscontrol/gTracking.js
