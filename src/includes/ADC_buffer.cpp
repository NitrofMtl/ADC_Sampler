#include "ADC_buffer.h"


AdcBuffer::AdcBuffer(uint8_t innerSize) : countInnerFront(0), countInnerRear(0), countOutterFront(0), countOutterRear(0) {
	value = new uint16_t*[BUFFER_SIZE];
	for ( int i = 0; i < BUFFER_SIZE; i++) {
		value[i] = new uint16_t[innerSize] {0};
	}
}

uint16_t* AdcBuffer::data() volatile {
	auto *arr = value[countOutterRear-1];
/*	if ( countOutterRear == 0 ) { 
		countOutterRear++;
	}*/
	Serial.print(" pos: "); Serial.print(countOutterRear);
	for ( int i = 0; i < 3; i++) {
		 Serial.print(" "); Serial.print(arr[i]);
	}
	Serial.println();
	
	//if ( (countOutterRear %10) == 0 ) Serial.println();
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
}

uint8_t AdcBuffer::arrearSize() volatile {
	return countOutterFront - countOutterRear;
}