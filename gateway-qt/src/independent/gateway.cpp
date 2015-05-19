

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


/**
 * @file gatewayGeneric.cc
 * @date 2012/05/01 7:07
 * @author Sergio Montenegro, Uni Würzburg
 *
 * Copyright 2009 DLR
 *
 * @brief gateway for middleware
 *
 */

#include "gateway-qt/api/gateway.h"
#include "QDebug"


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

long myNodeNr=3;
extern int64_t numberOfReceivedMsgsFromNetwork;



/**************** Transmitter part of the gateway   ******************/

int32_t globalMsgSequenceCounter = 1;

Gateway::Gateway(Linkinterface* linkinterface_, bool forwardall_, bool enable_)
    {
    this->linkinterface = linkinterface_;
    this->forwardAll = forwardall_;
    this->linkIdentifier = linkinterface_->getLinkdentifier();


    getTopicsToForwardFromOutside=true;

}

int32_t Gateway::numberOfNodes = 0;
SeenNode Gateway::seenNodes[MAX_NUMBER_OF_NODES];


int compare_SeenNode (const void *a, const void *b) {
    int32_t temp =  ((SeenNode*)a)->nodeID - ((SeenNode*)b)->nodeID;
    if(temp > 0)      return 1;
    else if(temp < 0) return -1;
    else              return 0;
}

bool Gateway::messageSeen(NetworkMessage& msg) {
    //SeenNode key;
    SeenNode* node;




    /*===========qsort and binsearch version, disabled for now ========================
    key.nodeID=msg.senderNode;

    node= (SeenNode*)bsearch(&key,seenNodes,numberOfNodes,sizeof(SeenNode),compare_SeenNode);

    if(!node) {
        if(numberOfNodes+1 > MAX_NUMBER_OF_NODES) {
            ERROR("There are more nodes than MAX_NUMBER_OF_NODES");
            return false;
        }
        key.lastMsgTime=msg.sentTime-1;
        seenNodes[numberOfNodes]=key;
        numberOfNodes++;

        qsort(seenNodes,numberOfNodes,sizeof(SeenNode),compare_SeenNode);
        node= (SeenNode*)bsearch(&key,seenNodes,numberOfNodes,sizeof(SeenNode),compare_SeenNode);
    }

    */

    //=============== simple search =======================================
       node=0;
       for(int i=0;i< numberOfNodes;i++){

           if(seenNodes[i].nodeID == msg.get_senderNode()){
               node = &seenNodes[i];
               break;
           }
       }

       if(!node){
           if(numberOfNodes > MAX_NUMBER_OF_NODES) {
               //ERROR("There are more nodes than MAX_NUMBER_OF_NODES");
               return false;
           }
           node=&seenNodes[numberOfNodes];
           numberOfNodes++;

           node->nodeID = msg.get_senderNode();
           node->lastMsgTime = msg.get_sentTime()-1;
       }

       // ==================================================


    int64_t msgSentTime = msg.get_sentTime();
    if(node->lastMsgTime >= msgSentTime) {
        if(node->lastMsgTime-msgSentTime > 5LL*1000LL*1000LL*1000LL) {
            node->lastMsgTime= msgSentTime;
            return false;
        }

        return true;
    } else {
        node->lastMsgTime= msgSentTime;

        return false;
    }

}

/** Forward the message to the interface **/
/*
long Gateway::put(const long topicId, const long len, const void* data, const NetMsgInfo& netMsgInfo) {
    if(!isEnabled) return 0;

    if(!forwardAll) {
        if(topicId !=0 && !externalsubscribers.find(topicId)) { return 0; }
    }

    networkOutProtector.enter();
    {
        prepareNetworkMessage(networkOutMessage,topicId,data,len);
        sendNetworkMessage(networkOutMessage);
    }
    networkOutProtector.leave();
    return 1;
}

*/


void Gateway::sendNetworkMessage(const char* data,int32_t len,uint32_t topicID,int64_t sendTime){
	prepareNetworkMessage(networkOutMessage,topicID,data,len,sendTime);
	linkinterface->sendNetworkMsg(networkOutMessage);
}









/****************  Receiver part of the gateway   ********************/




void Gateway::AnalyseAndDistributeMessagesFromNetwork() {

    if(networkInMessage.get_senderNode() == myNodeNr) {
        return;
    } // ***** discard messages from self

    if(!networkInMessage.isCheckSumOk()) {
        // PRINTF("Message dropped: Wrong Checksum %x != %x\n", networkInMessage.get_checksum(), networkInMessage.calculateCheckSum());
        return;
    }
    if(messageSeen(networkInMessage)) return;

    //networkInMessage.dec_maxStepsToForward();

    long topicId           = networkInMessage.get_topicId();

    if(topicId > 0) {
        /** now distribute locally (if not from self and not topicreports) **/


        NetMsgInfo msgInfo;
        msgInfo.linkId         = linkIdentifier;
        msgInfo.sentTime       = networkInMessage.get_sentTime();
        msgInfo.senderNode     = networkInMessage.get_senderNode();
        msgInfo.senderThreadId = networkInMessage.get_senderThreadId();

        if(putter){
        	putter->putGeneric(topicId,networkInMessage.get_len(),networkInMessage.userDataC,msgInfo);
        }
    }
}

/*************************************************************************/
/**
 * one trhead to distribute internaly the incommig messages
 */


void Gateway::init(){

	linkinterface->init();

}

void Gateway::pollMessages() {
    bool     didSomething;
    int32_t  realMsgSize;


        didSomething=true;
        while(didSomething) {
            didSomething=false;

            if(linkinterface->getNetworkMsg(networkInMessage, realMsgSize)) {

                if(realMsgSize < 0) {
                    AnalyseAndDistributeMessagesFromNetwork();
                } else {
                    if(realMsgSize >= ((int32_t)sizeof(NetworkMessage)-MAX_NETWORK_MESSAGE_LENGTH)) {
                        if(realMsgSize == (int32_t)networkInMessage.numberOfBytesToSend()) {
                            AnalyseAndDistributeMessagesFromNetwork();
                        } else {
//                            qDebug() <<"Message dropped because expected size(" << networkInMessage.numberOfBytesToSend() << " ) != real size(" << realMsgSize << ")";
                        }
                    } else {
//                        qDebug() << "Message dropped because received is even smaller than the header";
                    }
                }

                didSomething=true;
            }

        }

}




void prepareNetworkMessage(NetworkMessage& netMsg, const long topicId,const void* data, int len,int64_t sendTime) {
    netMsg.put_senderNode(myNodeNr); // Set node ID of sending node
    netMsg.put_topicId(topicId);     // ID of calling topic
    netMsg.put_sentTime(sendTime);      // Timestamp
    netMsg.put_maxStepsToForward(10);
    netMsg.put_senderThreadId(0);
    netMsg.setUserData(data, len);
    netMsg.setCheckSum();
}

//Counter for automaticaly assigning linkidentifiers
//This is here because Linkinterface has no own source file
int Linkinterface::linkidentifierCounter=10000;

#ifndef NO_RODOS_NAMESPACE
}
#endif

