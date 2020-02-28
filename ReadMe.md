# ADC_Sampler

### Enable the ADC DMA and timer interrupt to sample adc signal at high frequency without loading the MCU.

# Usage
- Import ADC_Sampler to library manager
- Include header, and declare how many adc channel you want to use:
````
#include "ADC_Sampler.h"

const uint8_t ADC_sequencer_size = 11; // MUST match the numer of input in ADC_Sample->begin()
````

- Then in setup, start the sampler with the frequency and add the channel in order that you want it to be store into the buffer. you could put one than once the same channel but the sampling inside the sequence is not exactly "sample frequency" / "number of channel".
````
void setup()
{
  //(double)sample frequency in Hz , ... channel in orfer into buffer
  ADC_Sampler::begin(200, A7,A6,A5,A4,A3,A2,A0,A1,A9,A10,A11);
}
````

you could check if new data is available:
````
if(ADC_Sampler::available()) {
}
````

To access data:
````
while ( ADC_Sampler::available() ) {
    uint16_t* x = ADC_Sampler::data(); // data() return a pointer array of the size of ADC_sequencer_size
    for (int i = 0; i < ADC_sequencer_size; i++) {
   x[i]; //  Do what you want with array data
    }
}
````

You could reset the buffer to make it catch the reading (useful after setup for example...). 
````
  ADC_Sampler::bufferReset(); //discard unread data in buffer for rear to catch front
````

- You can verify if your method is late versus the buffer:
````
Serial.println(ADC_Sampler::arrearSize()); // return the number of sample not treated yet to see if you can handle the sample rate
````

## Limitation

#### There is not overflow protection, if the reading full up the buffer before you extract the data, buffer is overwritten.

## New in version 1.1

### Begin function to use external sofware trigguer

If you want to synk ADC conversion with another signal, use beginExternalTrigger:

````
beginExternalTrigger(frequency, ... pinX);
````
Here, the frequency is ther expected frequenncy to adust the prescaler of the ADC. Should be smaller than the used handler.

Put inside your handler the call for conversion:
````
ADC_Sampler::startConversion();
````
