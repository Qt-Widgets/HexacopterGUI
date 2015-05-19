#ifndef HW_HAL_UART_UDP_H
#define HW_HAL_UART_UDP_H

#include <QUdpSocket>
#include <QObject>

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class HW_HAL_UART_UDP : public QObject{
    Q_OBJECT

public:

    explicit HW_HAL_UART_UDP(QObject *parent = 0);
    ~HW_HAL_UART_UDP();
    char buffer[1300];
    const char* antenna;
    unsigned int port;
    QUdpSocket *socket;
    bool dataReady;
    int length;

    void init(const char *localhost, const char *antenna, unsigned int port);

public slots:
    void readyRead();
};

#ifndef NO_RODOS_NAMESPACE
}
#endif


#endif // HW_HAL_UART_UDP_H
