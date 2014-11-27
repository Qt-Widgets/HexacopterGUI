#include <stdint.h>
#include "gateway-linux/api/stream-bytesex.h"
//#include "macros_bits_bytes.h"

/** Bytesex Convertions
 *  byte string allwas in Big-endian (motorola) format
 *  other formats in host representation
 *
 * Author    : Sergio Montenegro
 * Date      : 23.10.2008
 * Descript  : Byte-String to Values
 */

/************ From a gib-endian byte stram  to internal representation ****/

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

uint16_t bigEndianToInt16_t(const void* buff) {
    uint8_t* byteStream = (uint8_t*)buff;
    if (byteStream == 0) return 0;
    return   (((uint16_t)(byteStream[0])) << 8)
           |  ((uint16_t)(byteStream[1]));
}

uint32_t bigEndianToInt32_t(const void* buff) {
    uint8_t* byteStream = (uint8_t*)buff;
    if (byteStream == 0) return 0;
    return    (((uint32_t)(byteStream[0])) << 24)
            | (((uint32_t)(byteStream[1])) << 16)
            | (((uint32_t)(byteStream[2])) <<  8)
            |  ((uint32_t)(byteStream[3]));
}


uint64_t bigEndianToInt64_t(const void* buff) {
    uint8_t* byteStream = (uint8_t*)buff;
    if (byteStream == 0) return 0;
    return   (((uint64_t)(byteStream[0])) << 56)
           | (((uint64_t)(byteStream[1])) << 48)
           | (((uint64_t)(byteStream[2])) << 40)
           | (((uint64_t)(byteStream[3])) << 32)
           | (((uint64_t)(byteStream[4])) << 24)
           | (((uint64_t)(byteStream[5])) << 16)
           | (((uint64_t)(byteStream[6])) <<  8)
           |  ((uint64_t)(byteStream[7]));
}


float bigEndianToFloat(const void* buff) {
    uint8_t* byteStream = (uint8_t*)buff;
    union {
        float value;
        uint32_t lvalue;
    } value_union;
    value_union.lvalue = bigEndianToInt32_t(byteStream);
    return value_union.value;
}

double bigEndianToDouble(const void* buff) {
    uint8_t* byteStream = (uint8_t*)buff;
    union {
        double value;
        uint64_t llvalue;
    } value_union;
    value_union.llvalue = bigEndianToInt64_t(byteStream);
    return value_union.value;
}

/************ From internal representation to big-endian  byte stram  ****/

void int16_tToBigEndian(void* buff, uint16_t value) {
    uint8_t* byteStream = (uint8_t*)buff;
    if (byteStream == 0) return;
    byteStream[0] = (uint8_t) ((value >> 8) & 0xFF);
    byteStream[1] = (uint8_t) ((value >> 0) & 0xFF);
}

void int32_tToBigEndian(void* buff, uint32_t value) {
    uint8_t* byteStream = (uint8_t*)buff;
    if (byteStream == 0) return;
    byteStream[0] = (uint8_t) ((value >> 24) & 0xFF);
    byteStream[1] = (uint8_t) ((value >> 16) & 0xFF);
    byteStream[2] = (uint8_t) ((value >>  8) & 0xFF);
    byteStream[3] = (uint8_t) ((value >>  0) & 0xFF);
}


void int64_tToBigEndian(void* buff, uint64_t value) {
    uint8_t* byteStream = (uint8_t*)buff;
    if (byteStream == 0) return;
    byteStream[0] = (uint8_t) ((value >> 56) & 0xFF);
    byteStream[1] = (uint8_t) ((value >> 48) & 0xFF);
    byteStream[2] = (uint8_t) ((value >> 40) & 0xFF);
    byteStream[3] = (uint8_t) ((value >> 32) & 0xFF);
    byteStream[4] = (uint8_t) ((value >> 24) & 0xFF);
    byteStream[5] = (uint8_t) ((value >> 16) & 0xFF);
    byteStream[6] = (uint8_t) ((value >>  8) & 0xFF);
    byteStream[7] = (uint8_t) ((value >>  0) & 0xFF);
}

void floatToBigEndian(void* buff, float value_) {
    uint8_t* byteStream = (uint8_t*)buff;
    union {
        float value;
        unsigned lvalue;
    } value_union;
    value_union.value = value_;
    int32_tToBigEndian(byteStream, value_union.lvalue);
}

void doubleToBigEndian   (void* buff, double value_) {
    uint8_t* byteStream = (uint8_t*)buff;
    union {
        double value;
        uint64_t llvalue;
    } value_union;
    value_union.value = value_;
    int64_tToBigEndian(byteStream, value_union.llvalue);
}




#ifndef NO_RODOS_NAMESPACE
}
#endif


