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
        qcustomplot/qcustomplot.cpp \
        myitem.cpp \
        opticaltracking.cpp \
        gpscontrol/gpscoordinate.cpp \
        gpscontrol/gpstracking.cpp \
        gateway-qt/src/independent/checksumes.cpp \
        gateway-qt/src/independent/gateway.cpp \
        gateway-qt/src/independent/linkinterfaceuart.cpp \
        gateway-qt/src/independent/linkinterfaceuart_nb.cpp \
        gateway-qt/src/independent/linkinterfaceuart_udp.cpp \
        gateway-qt/src/independent/stream-bytesex.cpp \
        gateway-qt/src/qt/hal/hal_uart.cpp \
        gateway-qt/src/qt/hal/hal_uart_udp.cpp \
    gateway-qt/src/qt/hal/hw_hal_uart_udp.cpp \
    interfacedialog.cpp

HEADERS += mainwindow.h \
        copter.h \
        glwidget.h \
        datatypes.h \
        matlib.h \
        qcustomplot/qcustomplot.h \
        gateway-linux/api/gateway/topiclistreport.h \
        topics.h \
        myitem.h \
        opticaltracking.h \
        gpscontrol/gpscoordinate.h \
        gpscontrol/gpstracking.h \
        gateway-qt/api/gateway/gateway.h \
        gateway-qt/api/gateway/linkinterface.h \
        gateway-qt/api/gateway/linkinterfaceuart.h \
        gateway-qt/api/gateway/linkinterfaceuart_nb.h \
        gateway-qt/api/gateway/linkinterfaceuart_udp.h \
        gateway-qt/api/gateway/networkmessage.h \
        gateway-qt/api/hal/genericIO.h \
        gateway-qt/api/hal/hal_uart.h \
        gateway-qt/api/hal/hal_uart_udp.h \
        gateway-qt/api/checksumes.h \
        gateway-qt/api/fifo.h \
        gateway-qt/api/gateway.h \
        gateway-qt/api/hal.h \
        gateway-qt/api/putter.h \
        gateway-qt/api/stream-bytesex.h \
        gateway-qt/api/gateway/topiclistreport.h \
    gateway-qt/api/hal/hw_hal_uart_udp.h \
    interfacedialog.h

FORMS   += mainwindow.ui \
    interfacedialog.ui

LIBS += -L/usr/local/lib -lvrpn -lquat -pthread

RESOURCES += gpscontrol/gps.qrc \
            img/img.qrc

OTHER_FILES += gpscontrol/gTracking.html \
               gpscontrol/gTracking.js
