#include "ADC_buffer.h"


AdcBuffer::AdcBuffer(uint8_t innerSize) : countInnerFront(0), countInnerRear(0), countOutterFront(0), countOutterRear(0) {
	buffer = new uint16_t*[BUFFER_SIZE];
	for ( int i = 0; i < BUFFER_SIZE; i++) {
		buffer[i] = new uint16_t[innerSize] {0};
	}
}

uint16_t* AdcBuffer::get() volatile {
	auto *arr = buffer[countOutterRear];
//for troubleshoot
//	Serial.print(" pos: "); Serial.print(countOutterRear);
//	for ( int i = 0; i < 3; i++) {
//		 Serial.print(" "); Serial.print(arr[i]);
//	}
//	Serial.println();
	countOutterRear++;
	return arr;
} 

bool AdcBuffer::available() volatile {
if ( (countOutterRear+1) != countOutterFront ) return true;
		return false;
}

bool AdcBuffer::full() {
	if (countOutterRear == countOutterFront+2) return true;  //TO DO-->find how implement security
	return false;
}

void AdcBuffer::bufferReset() volatile {
	countOutterRear = countOutterFront-1; //discard data on buffer not already read.
}

uint8_t AdcBuffer::arrearSize() volatile {
	return countOutterFront - countOutterRear;
}