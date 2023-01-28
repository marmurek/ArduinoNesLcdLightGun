#include "Joystick.h"
#include "ButtonDebounce.h"

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK, 2, 0, false, false, false, false, false, false, false, false, false, false, false);

#define TRIGGER 6
#define SENSOR A6 //pin4

ButtonDebounce trigger(TRIGGER, 10);

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define PROBES 20
#define THRESHOLD 500

int buff[PROBES] = {0};
int counter = 0;
unsigned long total = 0;

void setup() {
//  Serial.begin(9600);

  pinMode(LED_BUILTIN_TX,OUTPUT);
  pinMode(LED_BUILTIN_RX,OUTPUT);

    Joystick.begin(false);

  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;

  for(int i=0; i< PROBES; i++)
  {
    buff[i] = analogRead(SENSOR);
    total += buff[i];
  }
}

int d0 = 0;
int d1 = 0;

void loop() {

  trigger.update();

  total -= buff[counter];
  buff[counter] = analogRead(SENSOR);
  total += buff[counter];

  counter += 1;

  if(counter == PROBES)
  {
    counter = 0;
  }

  auto avg = (total / PROBES);
  int d = avg > THRESHOLD;

//Serial.println(avg);
  
 if(trigger.state() != d0 || d != d1)
 {  
  d0 = trigger.state();
  d1 = d;

  digitalWrite(LED_BUILTIN_TX, 1 - d0);
  digitalWrite(LED_BUILTIN_RX, 1 - d1);
  
  Joystick.setButton(0, d0);
  Joystick.setButton(1, d1);
  
  Joystick.sendState();
 }
 
}
