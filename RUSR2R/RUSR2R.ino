#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
char sinetable [360];
int  i ;
//float fe=76923.0;
float fe=153846.0;
float f=32000.0;
float tau = 1/f;
float T = 359.0/fe;
float a = 1/fe/2.0/tau/T;
int nbImpulsionEmission = 10;
int nbInterruptionRecpetion = 2500; // 12metres max, temps max ecoute = 12/340, fe = 72000, 72000*12/340
boolean emission = true; // false = ecoute
int signal = 0; // signal reçu échantilionné

void ioinit (void)
{
  for (int i=0;i<359;i++)
  {
    //sinetable[i]=(char)(123.0+122.0*sin(2.0*3.14*(f+a*i)*(float)i/fe));
    sinetable[i]=(char)(123.0+122.0*sin(2.0*3.14*f*(float)i/fe));
    //sinetable[i]=i;
  }
  //Initialize output ports
  PORTD = B11111111;
  DDRD  = B11111111;
}
void timer_setup(){
ADMUX = B00100000;
//ADCSRA = B11101100;
ADCSRA = B11101011;
ADCSRB = B00000000;
DIDR0 = B00000000;
}

void setup(){          
//Serial.begin(9600);
Serial.begin(250000);
  ioinit();
  cli();
  timer_setup();
  i = 0;
  sei();
}

void timer_setup2(){
  //pinMode(marker, OUTPUT); // pin = output
  ADMUX = B01000011;            // measuring on ADC3, use the internal 1.1 reference
  ADCSRA =B10101011;           // AD-converter on, interrupt enabled, prescaler = 16
  ADCSRB =B01000000; 
  DIDR0 = B00111111;            // digital inputs disabled
          // AD channels MUX on, free running mode
  bitWrite(ADCSRA, 6, 1);  // Start the conversion by setting bit 6 (=ADSC) in ADCSRA
  sei();                   // set interrupt flag
}

ISR(ADC_vect) {
  if(emission){ 
    PORTD=(sinetable[i++]);   
    if(i==360){
      i=0;
      nbImpulsionEmission--;     
      if(nbImpulsionEmission==0){
        timer_setup2();
        emission = false;
        nbImpulsionEmission=10;
      }
    }
  }
  else{
    bitClear(PORTB, 4); // marker low
    signal = ADCL;        // store lower byte ADC
    signal += ADCH << 8;  // store higher bytes ADC
    //signal = ADCL;
    bitSet(PORTB, 4);   // marker high
    nbInterruptionRecpetion--;
    if(nbInterruptionRecpetion==0){
      timer_setup();
      nbInterruptionRecpetion=2500;
      emission = true;
    }  
  }
}

void loop()
{
  Serial.print("\t");
  Serial.println(signal);
}
