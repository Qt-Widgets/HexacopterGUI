/*********************************************************** Copyright
 **
 ** Copyright (c) 2008, German Aerospace Center (DLR)
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **
 ** 1 Redistributions of source code must retain the above copyright
 **   notice, this list of conditions and the following disclaimer.
 **
 ** 2 Redistributions in binary form must reproduce the above copyright
 **   notice, this list of conditions and the following disclaimer in the
 **   documentation and/or other materials provided with the
 **   distribution.
 **
 ** 3 Neither the name of the German Aerospace Center nor the names of
 **   its contributors may be used to endorse or promote products derived
 **   from this software without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 ****************************************************************************/

#pragma once

#include "gateway-qt/api/gateway/linkinterface.h"
#include "gateway-qt/api/gateway/gateway.h"
#include "gateway-qt/api/gateway/networkmessage.h"
#include "gateway-qt/api/hal/hal_uart_udp.h"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/**
 * @file linkinterfaceuart_udp.cpp
 * @date 2015/05/18
 * @author Michael Strohmeier
 *
 * Link Interface to uart.
 */



#define MAX_UART_MESSAGE_LENGTH (MAX_NETWORK_MESSAGE_LENGTH +6)

class LinkinterfaceUART_UDP : public Linkinterface, IOEventReceiver  {

    HAL_UART_UDP* uart;
    char msg[MAX_NETWORK_MESSAGE_LENGTH];
    //unsigned char inputBuffer[MAX_UART_MESSAGE_LENGTH]; // bom, topicid, message eom
    unsigned char* inputBuffer;
    enum ReadStates {
        WAITING_FOR_START,
        READING,
        ESCAPE
    } readState;
    int readIndex;

    volatile bool maySend; // controlled by ^s, ^q from reader

    NetworkMessage* inMsg;
    char currCharFromLastCall;
    char prevChar;
    int inputIndex;
    bool foundUART_BOM;

    volatile bool transmitinProgrss;

    char getcharWaiting();
    void putcharEncoded(const bool mark, const char c);
    bool processByte(char input);

public:

    LinkinterfaceUART_UDP(HAL_UART_UDP* uart, long int id = -1);
    void init();

    bool isNetworkMsgSent();
    bool sendNetworkMsg(NetworkMessage& outgoingMessage);
    bool getNetworkMsg(NetworkMessage &inMsg, int32_t &numberOfReceivedBytess);

    void sendFinished();
    void dataReady();
};


#ifndef NO_RODOS_NAMESPACE
}
#endif

