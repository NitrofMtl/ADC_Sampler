#include "ADC_Sampler.h"

const uint8_t ADC_sequencer_size = 11; // MUST match the numer of input in ADC_Sample->begin()

void setup()
{
  Serial.begin (115200) ; 

  //sample frequency, ... channel in orfer into buffer
  ADC_Sampler::begin(5, A7,A6,A5,A4,A3,A2,A0,A1,A9,A10,A11);
  delay(5000);
  pinMode(13, OUTPUT);
  ADC_Sampler::printSetup();

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
  delay(1);
}








/*
void loop()
{

  
  String output;
  if(ADC_Sampler::available()) Serial.println("A7,A6,A5,A4,A3,A2,A0,A1,A9,A10,A11");
  while ( ADC_Sampler::available() ) {
    uint8_t tmp = ADC_Sampler::bufferArray->countOutterRear;
    for (int i = 0; i < ADC_sequencer_size; i++) {
      
      uint16_t x =  ADC_Sampler::bufferArray->value[tmp][i];
      
      output += String(x);
      output += ",";  
    }
    
    output += " rear  ";
    output += String(tmp);
    output += "  front ";
    uint8_t fr = ADC_Sampler::bufferArray->countOutterFront;
    output += String(fr);    
    
    Serial.println(output);
    output = String(); //RESET
    ADC_Sampler::bufferArray->countOutterRear++;
  }
  delay(1);
}
*/