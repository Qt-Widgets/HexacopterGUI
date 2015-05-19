#include "gateway-qt/api/hal/hw_hal_uart_udp.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

HW_HAL_UART_UDP::HW_HAL_UART_UDP(QObject *parent) : QObject(parent){

}

HW_HAL_UART_UDP::~HW_HAL_UART_UDP(){

}

void HW_HAL_UART_UDP::init(const char *localhost, const char *antenna, unsigned int port)
{
    socket = new QUdpSocket();
    socket->bind(QHostAddress(localhost), port);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

    this->antenna = antenna;
    this->port = port;

    qDebug() << "Localhost: " << localhost;
    qDebug() << "Antenna:   " << antenna;
    qDebug() << "Port:      " << port;

}

void HW_HAL_UART_UDP::readyRead(){
    dataReady = true;
    length = socket->pendingDatagramSize();
    QHostAddress sender;
    quint16 senderPort;
    socket->readDatagram(buffer, length, &sender, &senderPort);
//    qDebug() << "Message from: " << sender.toString();
//    qDebug() << "Meesage port: " << senderPort;
//    qDebug() << "Message size  " << length;

}

#ifndef NO_RODOS_NAMESPACE
}
#endif
