#include "Arduino.h"
#include "ADC_Sampler.h"


uint8_t ADC_Sampler_class::numChannels = 0;


volatile AdcBuffer *ADC_Sampler_class::bufferArray = new AdcBuffer(ADC_sequencer_size);

void ADC_Sampler_class::TIAO_setup(uint32_t counter) {
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
	t->TC_CMR = ( (t->TC_CMR & ~(TC_CMR_LDRA(MAX_FIELD)) ) & (t->TC_CMR & ~(TC_CMR_LDRB(MAX_FIELD)) ) ) | TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_SET ;  // set clear and set from RA and RC compares

	t->TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG ;  // re-enable local clocking and switch to hardware trigger source.

	PIOB->PIO_PDR = PIO_PB25B_TIOA0 ;  // disable PIO control
	PIOB->PIO_IDR = PIO_PB25B_TIOA0 ;   // disable PIO interrupts
	PIOB->PIO_ABSR |= PIO_PB25B_TIOA0 ;  // switch to B peripheral

}

uint32_t ADC_Sampler_class::getClkFrequency(double f) {
	return VARIANT_MCK/2 /f;
}

void ADC_Sampler_class::ADC_init(uint8_t trigSel) {
	pmc_enable_periph_clk(ID_ADC);
	ADC->ADC_CR = ADC_CR_SWRST; //reset the adc
	ADC->ADC_IDR = MAX_FIELD ;   // disable interrupts
	NVIC_EnableIRQ (ADC_IRQn) ;   // enable ADC interrupt vector

	ADC->ADC_CHDR = 0xFFFF ;      // disable all channels*/
	ADC->ADC_CHER = 0;   //disable all channels

	ADC->ADC_MR = 0; //reset register
	ADC->ADC_MR |= ADC_MR_USEQ
				|  ADC_MR_TRGEN
				|  ADC_MR_TRGSEL(trigSel)
				|  ADC_MR_TRANSFER(1)
				|  ADC_MR_TRACKTIM(15)
				|  ADC_MR_SETTLING(0)
				|  ADC_MR_STARTUP(8)
				|  ADC_MR_PRESCAL(255)
				|  ADC_MR_TRANSFER(3);
}

uint8_t ADC_Sampler_class::numSettedChannel() {
	uint8_t setted = 0;
	for ( int i = 0; i < 15; i++) {
		if ( 1<<i & ADC->ADC_CHSR ) setted++;
	}
	return setted;
}

void ADC_Sampler_class::prescalerAdjust(uint32_t f) {
	uint32_t targetAdc = f * numChannels *CLOCK_CYCLE_PER_CONVERSION;
	double prescaler = VARIANT_MCK/2;	
	prescaler/=targetAdc;
	prescaler-=1;
	if (prescaler < 0) {
		Serial.println("SAMPLE FREQUENCY TO HIGH, ADC CLOCK CAN'T HANDLE IT !!! Reduce number of active channel or chose a lower frequency.");
		return;
	}
	prescaler = constrain(prescaler, 0, 255);
	ADC->ADC_MR &= ~(ADC_MR_PRESCAL(MAX_FIELD)); //reset prescaler
	ADC->ADC_MR |= ADC_MR_PRESCAL((int)prescaler);	//set prescaler
}

void ADC_Sampler_class::bufferConfig() {
	ADC->ADC_IER =  ADC_IDR_ENDRX;   // interrupt enable register, enables only ENDRX
 // following are the DMA controller registers for this peripheral
 // "receive buffer address" 
	ADC->ADC_RPR = (uint32_t)bufferArray->value[bufferArray->countOutterFront-1];   // DMA receive pointer register  points to beginning of global_ADCount
	// "receive count" 
	ADC->ADC_RCR = numChannels;  //  receive counter set
	// "next-buffer address"
	ADC->ADC_RNPR = (uint32_t)bufferArray->value[bufferArray->countOutterFront]; // next receive pointer register DMA global_ADCounts_Arrayfer  points to second set of data 
	// and "next count"
	ADC->ADC_RNCR = numChannels;   //  and next counter is set
	// "transmit control register"
	ADC->ADC_PTCR = ADC_PTCR_RXTEN;  // transfer control register for the DMA is set to enable receiver channel requests
}

uint16_t* ADC_Sampler_class::data() {
	uint16_t* arr = bufferArray->data();
	return arr;
}

bool ADC_Sampler_class::available() {
	if (bufferArray->countOutterRear != bufferArray->countOutterFront) return true;
	return false;
}

void ADC_Sampler_class::bufferReset() {
	bufferArray->bufferReset();
}

uint8_t ADC_Sampler_class::arrearSize() {
	return bufferArray->arrearSize();
}

void ADC_Sampler_class::startConversion() {
	ADC->ADC_CR = ADC_CR_START;
}

void ADC_Sampler_class::ADC_Handler() {     // for the ATOD: re-initialize DMA pointers and count	
	//   read the interrupt status register 
	if (ADC->ADC_ISR & ADC_ISR_ENDRX){ /// check the bit "endrx"  in the status register /// ADC_IDR_ENDRX correction
		/// set up the "next pointer register" 
		ADC->ADC_RNPR =(uint32_t)bufferArray->value[bufferArray->countOutterFront+1];  // next receive pointer register DMA global_ADCounts_Arrayfer  points to second set of data
		// set up the "next count"
		ADC->ADC_RNCR = numChannels;  // "receive next" counter
		bufferArray->countOutterFront++;
	}
}

void ADC_Sampler_class::printSetup() {
	TcChannel * t = &(TC0->TC_CHANNEL)[0];
	Serial.print("TC_TC  "); Serial.println(t->TC_RC);
}


ADC_Sampler_class ADC_Sampler;

#ifdef __cplusplus
extern "C"
{
#endif

void ADC_Handler (void)
{
	ADC_Sampler_class::ADC_Handler();
}

#ifdef __cplusplus
}
#endif