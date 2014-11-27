/*
 * Copyright (c) 2011 Uni Wuerzburg, Wuerzburg

 * All rights reserved.
 *
 * @author Michael Ruffer
 */
#include <gateway-linux/api/hal/hal_uart.h>
#include <qdebug.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>// for interrupt
#include <stdio.h>
#include <QSerialPort>

#include <gateway-linux/src/linux/hw_udp.h>
#include <gateway-linux/api/hal/hal_uart.h>

#define MAX_READ_CHUNK_SIZE 16

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif
//================================================================================
//Mapping of UART IDs to linux device names
#define MAX_NUM_UARTS 6
const char *uartDeviceNames[MAX_NUM_UARTS] = {"/dev/ttyUSB0","/dev/ttyUSB1","/dev/rfcomm0","/dev/rfcomm1","/dev/ttyS0","/dev/ttyAMA0"};
//================================================================================

class HW_HAL_UART {
public:
    bool charReady;
    char lastChar;
    int fd;

    UART_IDX idx;
    HAL_UART* hal_uart;
    QSerialPort* serial;

};

HW_HAL_UART UART_contextArray[MAX_NUM_UARTS];

HAL_UART::HAL_UART(UART_IDX uartIdx){
    context = &UART_contextArray[uartIdx];
    context->idx = uartIdx;
    context->hal_uart = this;
}


int HAL_UART::init(unsigned int iBaudrate) {

    context->serial = new QSerialPort();
    qDebug() << "Port name:" << uartDeviceNames[context->idx];
    qDebug() << "Baudrate:" << iBaudrate;
    context->serial->setPortName(uartDeviceNames[context->idx]);


    int baud = 0;
    switch(iBaudrate){
    case 9600: baud = QSerialPort::Baud9600;break;
    case 19200: baud = QSerialPort::Baud19200;break;
    case 38400: baud = QSerialPort::Baud38400;break;
    case 57600: baud = QSerialPort::Baud57600;break;
    case 115200: baud = QSerialPort::Baud115200;break;
    }

    context->serial->setBaudRate(baud);
    if(!context->serial->open(QIODevice::ReadWrite)){
        qDebug() << "Could not open device";
    }

    return 0;

}


int HAL_UART::config(UART_PARAMETER_TYPE type, int paramVal) {

    switch (type) {
    case UART_PARAMETER_BAUDRATE:
        if (paramVal > 0)
        {
            reset();
            init(paramVal);
        }
        else {return -1;}
        break;

    case UART_PARAMETER_HW_FLOW_CONTROL:
        return -1;

    case UART_ENABLE_DMA:
        return -1;

    default:
        return -1;
    }

    return 0;

}

void HAL_UART::reset(){

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

