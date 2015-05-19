#ifndef _HAL_UART_H_
#define _HAL_UART_H_


#include "genericIO.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


#define UART_BUF_SIZE 256

enum UART_PARAMETER_TYPE {
    UART_PARAMETER_BAUDRATE,
    UART_PARAMETER_HW_FLOW_CONTROL,
    UART_PARAMETER_ENABLE_DMA
//	UART_PARAMETER_RX_PIN,
//	UART_PARAMETER_TX_PIN,
//	UART_PARAMETER_RTS_PIN,
//	UART_PARAMETER_CTS_PIN
    //UART_PARAMETER_MODE
};


enum UART_STATUS_TYPE {
    UART_STATUS_RX_BUF_LEVEL,
    UART_STATUS_RX_ERROR
};

class HW_HAL_UART;


class HAL_UART : public GenericIOInterface {



private:

    HW_HAL_UART* context;




/* public functions */

public:
    HAL_UART();

    /* Initialization of uart interface: mode 8N1, no HW flow control*/
    int init(const char* port, unsigned int baudrate = 115200);

    /* disable interface and set all its registers and pins to its reset state */
    void reset();

    /* Configuration of uart interface AFTER initialization */
    int config(UART_PARAMETER_TYPE type, int paramVal);

    int status(UART_STATUS_TYPE type);


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

#endif // _HAL_UART_H_
