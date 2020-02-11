#include "ADC_buffer.h"



AdcBuffer::AdcBuffer(uint8_t innerSize) : countInnerFront(0), countInnerRear(0), countOutterFront(0), countOutterRear(0) {
	value = new uint16_t*[BUFFER_SIZE];
	for ( int i = 0; i < BUFFER_SIZE; i++) {
		value[i] = new uint16_t[innerSize];
	}
}


uint16_t* AdcBuffer::available() {
if (countOutterRear != countOutterFront){  ////<<<<<<<<<<<<<<---- addd move semantic
		uint8_t tmp = countOutterRear;
		countOutterRear++;
		return value[tmp];
	}
};


bool AdcBuffer::full() {
	if (countOutterRear == countOutterFront) return true;  //find how implement security
	return false;
}
