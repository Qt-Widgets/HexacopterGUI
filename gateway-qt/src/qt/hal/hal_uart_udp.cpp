/**
 * @file hal_uart_udp.cpp
 * @date 2015/18/05
 * @author Michael Strohmeier
 *
 * Copyright 2009 DLR, 2015 Uni Würzburg
 *
 * @brief HAL for UART interface.
 *
 */

#include <gateway-qt/api/hal/hal_uart_udp.h>
#include <gateway-qt/api/hal/hw_hal_uart_udp.h>
#include <qdebug.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

HW_HAL_UART_UDP hw_uart_udp;

HAL_UART_UDP::HAL_UART_UDP(){
    context = &hw_uart_udp;
}

int HAL_UART_UDP::init(const char *localhost, const char *antenna, unsigned int port) {
    context->init(localhost, antenna, port);
    return 0;
}

int HAL_UART_UDP::config() {

    return 0;

}

void HAL_UART_UDP::reset(){
    context->socket->abort();
}


int HAL_UART_UDP::read(char* buf, int size) {
    if(context->dataReady){
        memcpy(buf, context->buffer, size);
        if(size == context->length)
            context->dataReady = false;
        return size;
    }
    return 0;
}


int HAL_UART_UDP::write(const char* buf, int size) {
     return (int) context->socket->writeDatagram(buf, size, QHostAddress(context->antenna),context->port);
}


int HAL_UART_UDP::getcharNoWait() {
    static int pos = 0;
    if(pos == context->length){
        context->dataReady = false;
        pos = 0;
        context->length = 0;
        return -1;
    } else if(context->dataReady){
        return context->buffer[pos++];
    }
    return -1;
}


int HAL_UART_UDP::putcharNoWait(char c) {
    return -1;
}


bool HAL_UART_UDP::readyToSend() {
    return true;
}


bool HAL_UART_UDP::dataReady() {;
    return context->dataReady;
}



#ifndef NO_RODOS_NAMESPACE
}
#endif

