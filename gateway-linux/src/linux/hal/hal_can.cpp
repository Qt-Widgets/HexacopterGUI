/*
 * hal_can.cpp
 *
 *  Created on: 04.06.2013
 *      Author: Erik
 */

#include <gateway-linux/api/hal/hal_can.h>
#include <stdlib.h>
#include <string.h>


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif





HAL_CAN::HAL_CAN(CAN_IDX canIdx){

}

int HAL_CAN::init(unsigned int baudrate){



	return 0;


}

void HAL_CAN::reset(){

}

int HAL_CAN::config(CAN_PARAMETER_TYPE type, int paramVal){
	switch(type){
	case CAN_PARAMETER_BAUDRATE:
			reset();
			init(paramVal);
	return 0;


	}
	return -1;
}


int  HAL_CAN::status(CAN_STATUS_TYPE type){
	switch(type){
	case CAN_STATUS_RX_ERROR:
		return -1;
	case CAN_STATUS_TX_ERROR:
		return -1;
	}
	return -1;

}


bool  HAL_CAN::readyToSend(){
	return true;

}

bool  HAL_CAN::dataReady(){
	return true;
}

bool  HAL_CAN::addIncomingFilter(uint32_t ID, uint32_t IDMask, bool extID, bool rtr){

			return false;
}




int HAL_CAN::write(const char* sendBuf, int len,uint32_t canID,  bool extID, bool rtr){


	return 0;
}



int HAL_CAN::read(char* recBuf, uint32_t* canID, bool* isExtID,bool* rtr){
	return -1;

}



#ifndef NO_RODOS_NAMESPACE
}

#endif
