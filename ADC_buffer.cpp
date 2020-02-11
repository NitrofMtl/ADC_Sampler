#include "ADC_buffer.h"



AdcBuffer::AdcBuffer(uint8_t innerSize) : countInnerFront(0), countInnerRear(0), countOutterFront(0), countOutterRear(0) {
	value = new uint16_t*[BUFFER_SIZE];
	for ( int i = 0; i < BUFFER_SIZE; i++) {
		value[i] = new uint16_t[innerSize] {0};
	}
}

uint16_t* AdcBuffer::data() volatile {
	uint16_t *arr = value[countOutterRear];
	countOutterRear++;
	return arr;
} 


bool AdcBuffer::available() {
if (countOutterRear != countOutterFront) return true;
		return false;
}


bool AdcBuffer::full() {
	if (countOutterRear == countOutterFront) return true;  //find how implement security
	return false;
}
