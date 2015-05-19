/**
 * @file hal_uart.cpp
 * @date 2015/18/05
 * @author Michael Strohmeier
 *
 * Copyright 2009 DLR, 2015 Uni Würzburg
 *
 * @brief HAL for UART interface.
 *
 */

#include <gateway-qt/api/hal/hal_uart.h>
#include <qdebug.h>

#include <QSerialPort>

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class HW_HAL_UART{
public:
    QSerialPort *serial;
};

HW_HAL_UART hw_uart;

HAL_UART::HAL_UART(){
    context = &hw_uart;
}

int HAL_UART::init(const char *port, unsigned int baudrate) {
    context->serial = new QSerialPort();
    context->serial->setPortName(port);

    qDebug() << "Port name:" << port;
    qDebug() << "Baudrate:" << baudrate;

    int baud = 0;
    switch(baudrate){
    case 9600: baud = QSerialPort::Baud9600;break;
    case 19200: baud = QSerialPort::Baud19200;break;
    case 38400: baud = QSerialPort::Baud38400;break;
    case 57600: baud = QSerialPort::Baud57600;break;
    case 115200: baud = QSerialPort::Baud115200;break;
    }

    context->serial->setBaudRate(baud);
    if(!context->serial->open(QIODevice::ReadWrite)){
        qDebug() << "Could not open device";
        return -1;
    }

    return 0;
}

int HAL_UART::config(UART_PARAMETER_TYPE type, int paramVal) {
    int baud = 0;
    switch (type) {
    case UART_PARAMETER_BAUDRATE:
        switch(paramVal){
        case 9600: baud = QSerialPort::Baud9600;break;
        case 19200: baud = QSerialPort::Baud19200;break;
        case 38400: baud = QSerialPort::Baud38400;break;
        case 57600: baud = QSerialPort::Baud57600;break;
        case 115200: baud = QSerialPort::Baud115200;break;
        }
        context->serial->setBaudRate(baud);
        break;
    default:
        return -1;
    }

    return 0;

}

void HAL_UART::reset(){
    context->serial->close();
}


int HAL_UART::read(char* buf, int size) {
    return (int) context->serial->read(buf, size);
}


int HAL_UART::write(const char* buf, int size) {
    return (int) context->serial->write(buf, size);
}


int HAL_UART::getcharNoWait() {
    char c;
    if(context->serial->getChar(&c)){
        return (int) c;
    } else {
        return -1;
    }
}


int HAL_UART::putcharNoWait(char c) {
    if(context->serial->putChar(c)){
        return c;
    } else {
        return -1;
    }
}


int HAL_UART::status(UART_STATUS_TYPE type) {
    return 0;
}

bool HAL_UART::readyToSend() {
    context->serial->flush();
    return context->serial->bytesToWrite() == 0;
}


bool HAL_UART::dataReady() {
    return context->serial->bytesAvailable() > 0;
}



#ifndef NO_RODOS_NAMESPACE
}
#endif

