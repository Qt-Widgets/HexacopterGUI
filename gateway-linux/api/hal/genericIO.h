/*
 * Copyright (c) 2012 Uni Wuerzburg, Wuerzburg
 * All rights reserved.
 *
 * @author Sergio Montenegro, Michael Ruffer
 */

#ifndef GENERICIO_H_
#define GENERICIO_H_



#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class IOEventReceiver {
public:
	virtual void sendFinished() {}
	virtual void dataReady() { }
};

class GenericIOInterface {
private:
IOEventReceiver* ioEventReceiver;
protected:
	void* context;
public:
    GenericIOInterface() {ioEventReceiver=0; } // create all drivers static but do not init there

    /*** Implemented by each interface ****/
    virtual int init(...)   {return 0;}  // Virtual, but do not call using a pointer, each init differs
    virtual void reset() { }
    virtual int config(...) {return 0;}  // Virtual, but do not call using a pointer, each config differs
    virtual int status(...) {return 0;}  // Virtual, but do not call using a pointer, each status differs

    virtual bool readyToSend()  {return false; }
    virtual bool dataReady()    {return false; }

    virtual int write(const char* sendBuf, int len) 								{return 0;}
    virtual int read(char* recBuf, int maxlen) 										{return 0;}
    virtual int writeRead(const char* sendBuf, int len, char* recBuf, int maxlen)	{return 0;}

    /******* may be overridden by the user or received through IOEventREceiver, called by corrsponding driver  *****/
    virtual void upCallSendReady() {if(ioEventReceiver) ioEventReceiver->sendFinished(); } // last transmit order is concluded
    virtual void upCalldataReady() {if(ioEventReceiver) ioEventReceiver->dataReady();} // data has arrived
    void setIoEventReceiver(IOEventReceiver* receiver){
    	ioEventReceiver=receiver;
    }
};


#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* GENERICIO_H_ */
