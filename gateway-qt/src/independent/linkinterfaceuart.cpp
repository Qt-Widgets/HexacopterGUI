

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


#include "gateway-qt/api/gateway/linkinterfaceuart.h"
#include "gateway-qt/api/gateway/networkmessage.h"

/**
 * @file linkinterfaceuart.cc
 * @date 2012/10/26
 * @author Thomas Walter, uni Würzburg
 *
 * Link Interface to uart.
 *
 *
 */

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

LinkinterfaceUART::LinkinterfaceUART(HAL_UART* uart, long int id) : Linkinterface(id) {
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
void LinkinterfaceUART::putcharEncoded(const bool mark, const char c) {

    /** first check if we were stoped by control flow commands **/

    for(int i = 0; i < 5 && !maySend; i++) // ^s-> stop ? but with a limit
       // Thread::suspendCallerUntil(NOW() + 200*MILLISECONDS); // no more than 5 times, (one second)

    maySend = true; // in case the receiver is dead.

    /*** By the way, in linux do not need to send stop/continues...... */
    /** Not stoped -> send now **/
    if(mark) {
        myPutChar(MARK);
        myPutChar(c);
        return;						// <------------------------
    }

    if(c == MARK)  { // the only char for scrubbling
        myPutChar(MARK);
        myPutChar(ENCODED_FF);
        return;						// <------------------------
    }
    /** nothign special, the char may go out withoutn encoding **/
    myPutChar(c);

}

/*********************************************************************************/

bool LinkinterfaceUART::sendNetworkMsg(NetworkMessage& outgoingMessage)	{

    transmitinProgrss=true;
    int lenToSend = outgoingMessage.numberOfBytesToSend();
    char* dataToSend = (char*)&outgoingMessage;

    putcharEncoded(true, UART_BOM);
    for(int i = 0; i < lenToSend; i++) {
        putcharEncoded(false,dataToSend[i] );
    }
    putcharEncoded(true, UART_EOM);

    return true;
}


/************************************
 * UART Gateway                     *
 * This methode is for every incoming byte. It collects all recieved character and
 * decodes them to the S3P protocol. Once the EOM is recognized, the data is stored
 * in a NetworkMessage frame and it returns true when the message is complete.
 ************************************/

bool LinkinterfaceUART::processByte(char inputChar) {

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


void LinkinterfaceUART::init() { uart->setIoEventReceiver(this); }

bool LinkinterfaceUART::isNetworkMsgSent() { return !transmitinProgrss; }

bool LinkinterfaceUART::getNetworkMsg(NetworkMessage& inMsg,int32_t &numberOfReceivedBytes) {
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

void LinkinterfaceUART::sendFinished() {
    transmitinProgrss=false;
}

void LinkinterfaceUART::dataReady() {
}

#ifndef NO_RODOS_NAMESPACE
}
#endif