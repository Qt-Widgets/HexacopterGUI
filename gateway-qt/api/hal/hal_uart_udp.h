#ifndef HAL_UART_UDP_H
#define HAL_UART_UDP_H

#include "genericIO.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class HW_HAL_UART_UDP;

class HAL_UART_UDP : public GenericIOInterface {

private:

    HW_HAL_UART_UDP* context;


/* public functions */

public:
    HAL_UART_UDP();

    /* Initialization of uart interface: mode 8N1, no HW flow control*/
    int init(const char* localhost, const char* antenna, unsigned int port);

    /* disable interface and set all its registers and pins to its reset state */
    void reset();

    /* Configuration of uart interface AFTER initialization */
    int config();

    int status();


    bool readyToSend();
    bool dataReady();

    /* write()
     * blocking -> returns only if "sendBuf" is sent
     * TODO: transmit buffer & transmit interrupts for non-blocking operation
     */
    int write(const char* sendBuf, int len);
    /* read()
     * non-blocking -> returns when uart-receive-buffer is empty or "recBuf" is full
     */
    int read(char* recBuf, int maxLen);

    int getcharNoWait(); 		// returns character on success else -1
    int putcharNoWait(char c); 	// returns c on success else -1

};




#ifndef NO_RODOS_NAMESPACE
}
#endif


#endif // HAL_UART_UDP_H
