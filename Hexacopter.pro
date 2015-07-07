#-------------------------------------------------
#
# Project created by QtCreator 2014-11-12T09:48:07
#
#-------------------------------------------------

QT       += core gui opengl widgets serialport printsupport webkit webkitwidgets multimedia

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
        gateway-qt/src/independent/checksumes.cpp \
        gateway-qt/src/independent/gateway.cpp \
        gateway-qt/src/independent/linkinterfaceuart.cpp \
        gateway-qt/src/independent/linkinterfaceuart_nb.cpp \
        gateway-qt/src/independent/linkinterfaceuart_udp.cpp \
        gateway-qt/src/independent/stream-bytesex.cpp \
        gateway-qt/src/qt/hal/hal_uart.cpp \
        gateway-qt/src/qt/hal/hal_uart_udp.cpp \
        gateway-qt/src/qt/hal/hw_hal_uart_udp.cpp \
        interfacedialog.cpp \
        magnetic/Geomagnetism.cpp \
        Mapplot/copter2d.cpp \
        Mapplot/mapplot.cpp \
        Mapplot/ship.cpp \
        worldmagneticmodel.cpp \
    cameradialog.cpp \
    screen2.cpp \
    QcGaugeWidget/qcgaugewidget.cpp

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
        interfacedialog.h \
        magnetic/EGM9615.h \
        magnetic/Geomagnetism.h \
        Mapplot/copter2d.h \
        Mapplot/mapplot.h \
        Mapplot/ship.h \
        worldmagneticmodel.h \
    cameradialog.h \
    screen2.h \
    QcGaugeWidget/qcgaugewidget.h

FORMS   += mainwindow.ui \
    interfacedialog.ui \
    cameradialog.ui \
    screen2.ui

LIBS += -L/usr/local/lib -lvrpn -lquat -pthread

RESOURCES += \
            img/img.qrc

OTHER_FILES +=

INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib \
-lopencv_core \
-lopencv_imgproc \
-lopencv_highgui \
-lopencv_ml \
-lopencv_video \
-lopencv_features2d \
-lopencv_calib3d \
-lopencv_objdetect \
-lopencv_flann \
-lopencv_videoio
