#include "Arduino.h"

#ifndef ADC_BUFFER_H
#define ADC_BUFFER_H


#define BUFFER_SIZE 256


struct AdcBuffer {
	AdcBuffer(uint8_t innerSize);
	uint16_t **buffer;
	uint8_t countInnerFront:4;
	uint8_t countInnerRear:4;
	uint8_t countOutterFront;
	uint8_t countOutterRear;
	bool available() volatile;
	uint16_t* get() volatile;
	bool full();
	void bufferReset() volatile;
	uint8_t arrearSize() volatile;
};




#endif