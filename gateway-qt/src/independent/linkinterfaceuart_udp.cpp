/********************************************************* Copyright
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


#include "gateway-qt/api/gateway/linkinterfaceuart_udp.h"
#include "gateway-qt/api/gateway/networkmessage.h"

/**
 * @file linkinterfaceuart_udp.cpp
 * @date 2015/05/18
 * @author Michael Strohmeier
 *
 * Link Interface to uart.
 */

#include "QDebug"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

/*********************************************************************************/

static const char  UART_BOM         =  0x02;
static const char  UART_EOM         =  0x03;
static const char  ENCODED_FF		=  0x7E;
static const char  STOP_MARK		=  0x18;
static const char  CONTINUE_MARK	=  0x01;
static const char  MARK             =  (char)0xff;

/*********************************************************************************/

LinkinterfaceUART_UDP::LinkinterfaceUART_UDP(HAL_UART_UDP* uart, long int id) : Linkinterface(id) {
    this->uart=uart;
    // this->uart->init();
    readState = WAITING_FOR_START;
    readIndex = 0;
    maySend = true;
    currCharFromLastCall=0;
    prevChar=0;
    inputIndex=0;
    foundUART_BOM=false;
    transmitinProgrss=false;
}

/*********************************************************************************/

bool LinkinterfaceUART_UDP::sendNetworkMsg(NetworkMessage& outgoingMessage)	{

    transmitinProgrss=true;
    int dataLen = outgoingMessage.numberOfBytesToSend();
    char* dataToSend = (char*)&outgoingMessage;
    int idx = 0;
    msg[idx++] = MARK;
    msg[idx++] = UART_BOM;
    for(int i = 0; i < dataLen; i++) {
        if(dataToSend[i] == MARK){
             msg[idx++] = MARK;
             msg[idx++] = ENCODED_FF;
        } else {
            msg[idx++] = dataToSend[i];
        }
    }
    msg[idx++] = MARK;
    msg[idx++] = UART_EOM;

    int written = uart->write(msg, idx);
    qDebug() << "Test" << written;

    return true;
}


/************************************
 * UART Gateway                     *
 * This methode is for every incoming byte. It collects all recieved character and
 * decodes them to the S3P protocol. Once the EOM is recognized, the data is stored
 * in a NetworkMessage frame and it returns true when the message is complete.
 ************************************/

bool LinkinterfaceUART_UDP::processByte(char inputChar) {

    prevChar = currCharFromLastCall;
    currCharFromLastCall = inputChar;

    if(inputChar == MARK)	{ return false; } 	// <------ 	fetch next char, ff alone does not help

    if(prevChar == MARK && inputChar == UART_EOM) {	  // end of message detected
        foundUART_BOM = false;
        return true;
    }

    if(prevChar == MARK && inputChar == UART_BOM) {		//Begin of networkmessage detected
        foundUART_BOM = true;
        inputIndex = 0;
        return false;
    }
    if(prevChar == MARK && inputChar == STOP_MARK) {
        maySend = false;
        return false;
    }
    if(prevChar == MARK && inputChar == CONTINUE_MARK) {
        maySend = true;
        return false;
    }
    if(prevChar == MARK && inputChar == ENCODED_FF) {
        if(foundUART_BOM) {
            inputBuffer[inputIndex] = MARK;
            if(inputIndex < MAX_UART_MESSAGE_LENGTH - 1 ) inputIndex++;
        }
        return false;
    }

    if(prevChar == MARK)  return false; 	// this is an unknown code! just discard
    if(currCharFromLastCall == MARK) return false; 	// <------ 	fetch next char, ff alone does not help

    inputBuffer[inputIndex] = inputChar;	// place recieved char in inputbuffer
    if(inputIndex < MAX_UART_MESSAGE_LENGTH - 1) inputIndex++;

    return false;
}


void LinkinterfaceUART_UDP::init() { uart->setIoEventReceiver(this); }

bool LinkinterfaceUART_UDP::isNetworkMsgSent() { return !transmitinProgrss; }

bool LinkinterfaceUART_UDP::getNetworkMsg(NetworkMessage& inMsg,int32_t &numberOfReceivedBytes) {
    inputBuffer = (unsigned char*)&inMsg;
    while(uart->dataReady()) {
        char c = uart->getcharNoWait();
        if(processByte(c)) {
            numberOfReceivedBytes=inputIndex;
            return true;
        }
    }
    return false;
}

void LinkinterfaceUART_UDP::sendFinished() {
    transmitinProgrss=false;
}

void LinkinterfaceUART_UDP::dataReady() {
}

#ifndef NO_RODOS_NAMESPACE
}
#endif
