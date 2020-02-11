#include "Arduino.h"

#ifndef ADC_BUFFER_H
#define ADC_BUFFER_H


#define BUFFER_SIZE 256


struct AdcBuffer {
	AdcBuffer(uint8_t innerSize);
	uint16_t **value;
	uint8_t countInnerFront:4;
	uint8_t countInnerRear:4;
	uint8_t countOutterFront:16;
	uint8_t countOutterRear:16;
	uint16_t* available();
	bool full();
};




#endif