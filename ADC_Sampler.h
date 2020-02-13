#ifndef ADC_Sampler_h
#define ADC_Sampler_h

#include "Arduino.h"

#include "ADC_buffer.h"

#if defined(ARDUINO_ARCH_AVR)
  	#error "This libraries is for arduino DUE only !!!"
#elif defined(ARDUINO_ARCH_SAM)
  // SAM-specific code


#ifndef BIT_FIELD
#define BIT_FIELD(field)   MAX_FIELD >> (32-field)
#define MAX_FIELD (uint32_t)-1
#endif


#ifndef ADC_MR_FIELD
#define ADC_MR_FIELD
#define ADC_MR_TRANSFER(x) ( x & BIT_FIELD(2) ) << 28
#define ADC_MR_TRACKTIM(x) ( x & BIT_FIELD(4) ) << 24
#define ADC_MR_SETTLING(x) ( x & BIT_FIELD(2) ) << 20
#define ADC_MR_STARTUP(x)  ( x & BIT_FIELD(4) ) << 16
#define ADC_MR_PRESCAL(x)  ( x & BIT_FIELD(8) ) << 8
#define ADC_MR_TRGSEL(x)   ( x & BIT_FIELD(3) ) << 1
#endif

#ifndef TC_CMR_FIELD
#define TC_CMR_FIELD
#define TC_CMR_LDRA(x) ( x & BIT_FIELD(2) ) << 16
#define TC_CMR_LDRB(x) ( x & BIT_FIELD(2) ) << 18
#endif

#define INTERNAL_TEMP 13

#define CLOCK_CYCLE_PER_CONVERSION 20

extern const uint8_t ADC_sequencer_size;


const adc_channel_num_t adcChannel[] {
										ADC_CHANNEL_7,
										ADC_CHANNEL_6,
										ADC_CHANNEL_5,
										ADC_CHANNEL_4,
										ADC_CHANNEL_3,
										ADC_CHANNEL_2,
										ADC_CHANNEL_1,
										ADC_CHANNEL_0,
										ADC_CHANNEL_10,
										ADC_CHANNEL_11,
										ADC_CHANNEL_12,
										ADC_CHANNEL_13,
										ADC_CHANNEL_14,
										ADC_TEMPERATURE_SENSOR
									 };

static void enableChX(uint8_t pin){ 
	if(pin==52) pin=12; //PIN 52 (AD14)  
	else if(pin>=A0) pin-=A0;  //shift analog pin to int
	if(pin>13) return;  //ignore input bigger than register
	if(pin==INTERNAL_TEMP+2) ADC->ADC_ACR |= ADC_ACR_TSON; //enable internal temp sensor
	uint32_t ch = 0;
	while ((1<<ch) & ADC->ADC_CHSR) ch++; //find first free spot in channel register
	
	ADC->ADC_CHER |= (1<<ch); //enale next channel
	if ( ch < 8 ) {
		if (ch==0) {
			ADC->ADC_SEQR1 |= adcChannel[pin];
			return;
		}
		ADC->ADC_SEQR1 |= adcChannel[pin] << ch*4; //fill up sequencer 1
		return;
	}
	ch-=8;
	
	if (ch==0) {
		ADC->ADC_SEQR2 |= adcChannel[pin];
		return;
	}
	ADC->ADC_SEQR2 |= adcChannel[pin] << ch*4; //fill up sequencer 2*/
};

template<typename Pin, typename ... PinX>
static void enableChX(Pin pin, PinX ... pinX) { enableChX((uint8_t)pin); enableChX((uint8_t)pinX...); };


class ADC_Sampler {
private:
	static void TIAO_setup(uint32_t counter);
	static uint32_t getClkFrequency(double f);
	static void ADC_init();
	static uint8_t numSettedChannel();
	static void prescalerAdjust(uint32_t f);
	
public:
	static void bufferConfig();
	static uint8_t numChannels;
	static volatile AdcBuffer *bufferArray; //one dinamic alloc for each enable channel
	static void ADC_Handler();
	static uint16_t *data();
	static bool available();
	static void printSetup();

	template<typename ... PinX>
	static void begin(double f, PinX ... pinX){
		uint32_t counter = getClkFrequency(f);
		TIAO_setup(counter);
		ADC_init();
		enableChX(pinX...);
		numChannels = numSettedChannel();
		prescalerAdjust(f);
		bufferConfig();
	};

};


#endif

#endif


















/*
begin: configs timer, intterrup and selected input

handler, add to buffer - receive buffer
					   - find right buffer
					   -buffer increment
extract data : copy / move semantic

destructor

*/