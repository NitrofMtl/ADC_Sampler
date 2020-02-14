#include "ADC_Sampler.h"

const uint8_t ADC_sequencer_size = 11; // MUST match the numer of input in ADC_Sample->begin()

void setup()
{
  Serial.begin (115200) ; 

  //(double)sample frequency in Hz , ... channel in orfer into buffer
  ADC_Sampler::begin(200, A7,A6,A5,A4,A3,A2,A0,A1,A9,A10,A11);
  delay(5000);
  ADC_Sampler::bufferReset(); //discard unread data in buffer for rear to catch front
}



void loop() {
  String output;
  if(ADC_Sampler::available()) Serial.println("A7,A6,A5,A4,A3,A2,A0,A1,A9,A10,A11");
  while ( ADC_Sampler::available() ) {
    uint16_t* x = ADC_Sampler::data();
    for (int i = 0; i < ADC_sequencer_size; i++) {
      output += String(x[i]);
      output += ",";
    }
    
    output += " rear  ";
    output += String(ADC_Sampler::bufferArray->countOutterRear);
    output += "  front ";
    uint8_t fr = ADC_Sampler::bufferArray->countOutterFront;
    output += String(fr); 
    Serial.println(output);   
    output = String();//RESET
  }
  delay(1000); //here delay is exagerated to demonstrate buffer monitor overflow
  Serial.print("Data sample buffered: ");
  Serial.println(ADC_Sampler::arrearSize()); // return the number of sample not treated yet to see if you can handle the sample rate
  //there is not overflow protection, if the reading full up the buffer before you extract the data, buffer is overwritten.
}