#include "ADC_buffer.h"



AdcBuffer::AdcBuffer(uint8_t innerSize) : countInnerFront(0), countInnerRear(0), countOutterFront(0), countOutterRear(0) {
	value = new uint16_t*[BUFFER_SIZE];
	for ( int i = 0; i < BUFFER_SIZE; i++) {
		value[i] = new uint16_t[innerSize] {0};
	}
}

uint16_t* AdcBuffer::data() volatile {
	uint16_t *arr = value[countOutterRear-1];
	countOutterRear++;
	return arr;
} 


bool AdcBuffer::available() {
if (countOutterRear != countOutterFront) return true;
		return false;
}


bool AdcBuffer::full() {
	if (countOutterRear == countOutterFront+2) return true;  //TO DO-->find how implement security
	return false;
}

void AdcBuffer::bufferReset() volatile {
	countOutterRear = countOutterFront-1; //discard data on buffer not already read.
	Serial.println(countOutterFront);
}

uint8_t AdcBuffer::arrearSize() volatile {
	return countOutterFront - countOutterRear;
}