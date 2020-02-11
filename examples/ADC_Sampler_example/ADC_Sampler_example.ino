#include "ADC_Sampler.h"


ADC_Sampler sampler;

void setup()
{
  Serial.begin (115200) ; // was for debugging
  delay(1000);
  adc_setup () ;         // setup ADC
  //sample frequency, channel in orfer into buffer... 
  ADC_Sampler::begin(44100, 7,6,5,4,3,2,0,1,9,10,11);
  ADC_Sampler::bufferConfig();
 /*
  pmc_enable_periph_clk (TC_INTERFACE_ID + 0*3+0) ;  // clock the TC0 channel 0

  TcChannel * t = &(TC0->TC_CHANNEL)[0] ;    // pointer to TC0 registers for its channel 0
  t->TC_CCR = TC_CCR_CLKDIS ;  // disable internal clocking while setup regs
  t->TC_IDR = 0xFFFFFFFF ;     // disable interrupts
  t->TC_SR ;                   // read int status reg to clear pending
  t->TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK1 |   // use TCLK1 (prescale by 2, = 42MHz)
              TC_CMR_WAVE |                  // waveform mode
              TC_CMR_WAVSEL_UP_RC |          // count-up PWM using RC as threshold
              TC_CMR_EEVT_XC0 |     // Set external events from XC0 (this setup TIOB as output)
              TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_CLEAR |
              TC_CMR_BCPB_CLEAR | TC_CMR_BCPC_CLEAR ;
 
  t->TC_RC =  875 ;     // counter resets on RC, so sets period in terms of 42MHz clock---952 for 44.1kHz
  t->TC_RA =  440 ;     // roughly square wave
  t->TC_CMR = (t->TC_CMR & 0xFFF0FFFF) | TC_CMR_ACPA_CLEAR | TC_CMR_ACPC_SET ;  // set clear and set from RA and RC compares
 
  t->TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG ;  // re-enable local clocking and switch to hardware trigger source.
*/
  //setup_pio_TIOA0 () ;  // drive Arduino pin 2 at 48kHz to bring clock out
  //dac_setup () ;        // setup up DAC auto-triggered at 48kHz
}

void setup_pio_TIOA0 ()  // Configure Ard pin 2 as output from TC0 channel A (copy of trigger event)
{/*
  PIOB->PIO_PDR = PIO_PB25B_TIOA0 ;  // disable PIO control
  PIOB->PIO_IDR = PIO_PB25B_TIOA0 ;   // disable PIO interrupts
  PIOB->PIO_ABSR |= PIO_PB25B_TIOA0 ;  // switch to B peripheral*/
}


void dac_setup ()
{
/*  pmc_enable_periph_clk (DACC_INTERFACE_ID) ; // start clocking DAC
  DACC->DACC_CR = DACC_CR_SWRST ;  // reset DAC

  DACC->DACC_MR =
    DACC_MR_TRGEN_EN | DACC_MR_TRGSEL (1) |  // trigger 1 = TIO output of TC0
    (0 << DACC_MR_USER_SEL_Pos) |  // select channel 0
    DACC_MR_REFRESH (0x0F) |       // bit of a guess... I'm assuming refresh not needed at 48kHz
    (24 << DACC_MR_STARTUP_Pos) ;  // 24 = 1536 cycles which I think is in range 23..45us since DAC clock = 42MHz

  DACC->DACC_IDR = 0xFFFFFFFF ; // no interrupts
  DACC->DACC_CHER = DACC_CHER_CH0 << 0 ; // enable chan0*/
}

void dac_write (int val)
{
  //DACC->DACC_CDR = val & 0xFFF ;
}



void adc_setup ()
{
/*  NVIC_EnableIRQ (ADC_IRQn) ;   // enable ADC interrupt vector
  ADC->ADC_IDR = 0xFFFFFFFF ;   // disable interrupts
  ADC->ADC_IER = 0x80 ;         // enable AD7 End-Of-Conv interrupt (Arduino pin A0)
  ADC->ADC_CHDR = 0xFFFF ;      // disable all channels
  ADC->ADC_CHER = 0x80 ;        // enable just A0
  ADC->ADC_CGR = 0x15555555 ;   // All gains set to x1
  ADC->ADC_COR = 0x00000000 ;   // All offsets off
 
  ADC->ADC_MR = (ADC->ADC_MR & 0xFFFFFFF0) | (1 << 1) | ADC_MR_TRGEN ;  // 1 = trig source TIO from TC0*/
}

// Circular buffer, power of two.
#define BUFSIZE 0x400
#define BUFMASK 0x3FF
volatile int samples [BUFSIZE] ;
volatile int sptr = 0 ;
volatile int isr_count = 0 ;   // this was for debugging

/*
#ifdef __cplusplus
extern "C"
{
#endif

void ADC_Handler (void)
{
  if (ADC->ADC_ISR & ADC_ISR_EOC7)   // ensure there was an End-of-Conversion and we read the ISR reg
  {
    int val = *(ADC->ADC_CDR+7) ;    // get conversion result
    samples [sptr] = val ;           // stick in circular buffer
    sptr = (sptr+1) & BUFMASK ;      // move pointer
    dac_write (0xFFF & ~val) ;       // copy inverted to DAC output FIFO
  }
  isr_count ++ ;
}

#ifdef __cplusplus
}
#endif
*/

void loop()
{
}