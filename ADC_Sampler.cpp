#include "Arduino.h"
#include "ADC_Sampler.h"




AdcBuffer::AdcBuffer(uint8_t innerSize) : countInnerFront(0), countInnerRear(0), countOutterFront(0), countOutterRear(0) {
	value = new uint16_t*[BUFFER_SIZE];
	for ( int i = 0; i < BUFFER_SIZE; i++) {
		value[i] = new uint16_t[innerSize];
	}
}

/*uint16_t AdcBuffer::*availabble() {
	//if (countOutterRear != countOutterFront) {
		//uint8_t tmp = countOutterRear;
	//}
	//	
	//	countOutterRear++;
		//return value[tmp];
}*/


bool AdcBuffer::full() {
	if (countOutterRear == countOutterFront) return true;  //find how implement security
	return false;
}



uint8_t ADC_Sampler::numChannels = 0;

AdcBuffer ADC_Sampler::*bufferArray;

void ADC_Sampler::TIAO_setup(uint16_t counter) {
	pmc_enable_periph_clk (TC_INTERFACE_ID + 0*3+0) ;  // clock the TC0 channel 0

	TcChannel * t = &(TC0->TC_CHANNEL)[0] ;    // pointer to TC0 registers for its channel 0
	t->TC_CCR = TC_CCR_CLKDIS ;  // disable internal clocking while setup regs
	t->TC_IDR = MAX_FIELD ;     // disable interrupts
	t->TC_SR ;                   // read int status reg to clear pending
	t->TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK1 |   // use TCLK1 (prescale by 2, = 42MHz)
	        	TC_CMR_WAVE |                  // waveform mode
	        	TC_CMR_WAVSEL_UP_RC |          // count-up PWM using RC as threshold
	        	TC_CMR_EEVT_XC0 |     // Set external events from XC0 (this setup TIOB as output)
	        	TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_CLEAR |
	        	TC_CMR_BCPB_CLEAR | TC_CMR_BCPC_CLEAR ;

	t->TC_RC =  counter ;     // counter resets on RC, so sets period in terms of 42MHz clock---952 for 44.1kHz
	t->TC_RA =  counter/2 ;     // roughly square wave
	//t->TC_CMR = (t->TC_CMR & 0xFFF0FFFF) | TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_SET ;  // set clear and set from RA and RC compares
	t->TC_CMR =  (t->TC_CMR & ~(TC_CMR_LDRA(MAX_FIELD)) ) & (t->TC_CMR & ~(TC_CMR_LDRB(MAX_FIELD)) ) | TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_SET ;  // set clear and set from RA and RC compares

	t->TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG ;  // re-enable local clocking and switch to hardware trigger source.

	PIOB->PIO_PDR = PIO_PB25B_TIOA0 ;  // disable PIO control
	PIOB->PIO_IDR = PIO_PB25B_TIOA0 ;   // disable PIO interrupts
	PIOB->PIO_ABSR |= PIO_PB25B_TIOA0 ;  // switch to B peripheral

}

uint16_t ADC_Sampler::getClkFrequency(float f) {
	return VARIANT_MCK/2 /f;
}

void ADC_Sampler::ADC_init() {
	pmc_enable_periph_clk(ID_ADC);
	ADC->ADC_CR |= ADC_CR_SWRST; //reset the adc
	ADC->ADC_IDR = MAX_FIELD ;   // disable interrupts
	NVIC_EnableIRQ (ADC_IRQn) ;   // enable ADC interrupt vector
//	ADC->ADC_IER =  ADC_IDR_ENDRX;   // interrupt enable register, enables only ENDRX//////////////////mae crash... maybe function not setted param///////////////////////////////////////////////////////////////////////////////////////////////////
	ADC->ADC_CHDR = 0xFFFF ;      // disable all channels*/
	////////ADC->ADC_CHER = 0x80 ;        // enable just A0  //////////////////to add methode of enable
	ADC->ADC_CHER = 0;   //disable all channels

	ADC->ADC_MR = 0; //reset register
	ADC->ADC_MR |= ADC_MR_USEQ
				|  ADC_MR_TRGEN
				|  ADC_MR_TRGSEL(1)
				|  ADC_MR_TRANSFER(1)
				|  ADC_MR_TRACKTIM(15)
				|  ADC_MR_SETTLING(0)
				|  ADC_MR_STARTUP(8)
				|  ADC_MR_PRESCAL(255)
				|  ADC_MR_TRANSFER(3);
}

uint8_t ADC_Sampler::numSettedChannel() {
	uint8_t setted = 0;
	for ( int i = 0; i < 15; i++) {
		if ( 1<<i & ADC->ADC_CHSR ) setted++;
	}
	return setted;
}

void ADC_Sampler::prescalerAdjust(uint32_t f) {
	uint32_t targetAdc = f * numChannels *CLOCK_CYCLE_PER_CONVERSION;
	double prescaler = VARIANT_MCK/2;	
	prescaler/=targetAdc;
	prescaler-=1;
	if (prescaler < 0) {
		Serial.println("SAMPLE FREQUENCY TO HIGH, ADC CLOCK CAN'T HANDLE IT !!! Reduce number of actie channel or chose a lower frequency.");
		return;
	}
	ADC->ADC_MR &= ~(ADC_MR_PRESCAL(MAX_FIELD)); //reset prescaler
	ADC->ADC_MR |= ADC_MR_PRESCAL((int)prescaler);	//set prescaler
}

void ADC_Sampler::bufferConfig() {
	bufferArray = new AdcBuffer(numChannels);
	/*
 // following are the DMA controller registers for this peripheral
 // "receive buffer address" 
	ADC->ADC_RPR = (uint32_t)global_ADCounts_Array;   // DMA receive pointer register  points to beginning of global_ADCount
	// "receive count" 
	ADC->ADC_RCR = numChannels;  //  receive counter set
	// "next-buffer address"
	ADC->ADC_RNPR = (uint32_t)global_ADCounts_Array; // next receive pointer register DMA global_ADCounts_Arrayfer  points to second set of data 
	// and "next count"
	ADC->ADC_RNCR = numChannels;   //  and next counter is set
	// "transmit control register"
	ADC->ADC_PTCR = ADC_PTCR_RXTEN;  // transfer control register for the DMA is set to enable receiver channel requests
	*/
}

//void configBufferHelper() { ADC_Sampler::bufferArray = new AdcBuffer(ADC_Sampler::numChannels);};


#ifdef __cplusplus
extern "C"
{
#endif

void ADC_Handler (void)
{
/*  if (ADC->ADC_ISR & ADC_ISR_EOC7)   // ensure there was an End-of-Conversion and we read the ISR reg
  {
    int val = *(ADC->ADC_CDR+7) ;    // get conversion result
    samples [sptr] = val ;           // stick in circular buffer
    sptr = (sptr+1) & BUFMASK ;      // move pointer
    dac_write (0xFFF & ~val) ;       // copy inverted to DAC output FIFO
  }
  isr_count ++ ;*/
}

#ifdef __cplusplus
}
#endif