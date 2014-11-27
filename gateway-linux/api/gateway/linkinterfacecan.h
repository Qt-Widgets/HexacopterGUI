

#pragma once

#include <gateway-linux/api/gateway/linkinterface.h>
#include <gateway-linux/api/fifo.h>
#include <gateway-linux/api/hal.h>
#include <gateway-linux/api/hal/hal_can.h>



#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif




#define CAN_LINK_NODE_BITS 8
#define CAN_LINK_TOPIC_BITS 16
#define CAN_LINK_ID (0b11100 <<(CAN_LINK_NODE_BITS+CAN_LINK_TOPIC_BITS))

//Number of CAN messages to buffer when multiple meessages are interleaved during receiving
#define CAN_MSG_BUFFER_SIZE 128


struct BufferedCANMessage {
	uint32_t canID;
	BufferedCANMessage* next;
	uint8_t data[8];
	int8_t len;
};



class LinkinterfaceCAN : public Linkinterface, IOEventReceiver {

	HAL_CAN& can;

	BufferedCANMessage BufferedCANMessage_memory[CAN_MSG_BUFFER_SIZE];
	BufferedCANMessage* emptyBufferedCANMessages[CAN_MSG_BUFFER_SIZE];
	int emptyBufferedCANMessagesPos;
	BufferedCANMessage* bufferedCANMsgs_start;
	BufferedCANMessage* bufferedCANMsgs_end;

	NetworkMessage* currentMsg;
	uint32_t currentReceiveCANId;
	uint8_t* currentDataPointer;
	int bytesReceived;
	int sequenceCounter;

	BufferedCANMessage* receiveCANMessage();
	bool appendCANMsgToCurrentNetMsg(BufferedCANMessage* canMsg);


public:

    LinkinterfaceCAN(HAL_CAN* _can);


    void init();

    virtual bool sendNetworkMsg(NetworkMessage& outMsg);
    bool getNetworkMsg(NetworkMessage &inMsg,int32_t &numberOfReceivedBytes);

    void sendFinished();
    void dataReady();


};

#ifndef NO_RODOS_NAMESPACE
}
#endif

