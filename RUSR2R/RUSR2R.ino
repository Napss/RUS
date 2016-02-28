#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
char sinetable [360];
int  i ;
float fe=76923.0;
float f=4000.0;

float tau = 1/f;
float T = 359.0/fe;
float a = 1/fe/2.0/tau/T;
void ioinit (void)
{
  for (int i=0;i<359;i++)
  
  {
    sinetable[i]=(char)(123.0+122.0*sin(2.0*3.14*f+a*i));
  }
  //Initialize output ports
  PORTD = B11111111;
  DDRD  = B11111111;
}

void timer_setup(){
ADMUX = B00100000;
ADCSRA = B11101100;
ADCSRB = B00000000;
DIDR0 = B00000000;

}
void setup(){          
Serial.begin(9600);
  ioinit();
  cli();
  timer_setup();
  i = 0;
  sei();

}


ISR(ADC_vect) {

  PORTD=(sinetable[i++]);
  if(i==360){
    i=0;
  }

}
void loop()
{
}
