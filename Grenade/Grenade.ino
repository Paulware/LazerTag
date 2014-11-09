#include <TimerOne.h>
#include <IR.h>
#include <IRLazer.h>
#include <MemoryFree.h>

#define IRRECEIVEPIN 10
#define IRTRANSMIT 3
#define REPAIRROBOT false

IRLazer ir (IRRECEIVEPIN, IRTRANSMIT, &Timer1, REPAIRROBOT);
unsigned long timeout = 0;
unsigned long fireTimeout = 0;
unsigned long ceaseFire = 0;

void setup() {
  ir.init(); 
  
  pinMode (5,INPUT);  // Switch input
  pinMode (3,OUTPUT); // IR Tx
  pinMode (6,OUTPUT); // Sound module go
  digitalWrite (5,1); // pull-up internal resistor
  Serial.begin (115200);
  Serial.println ( "Ready" );
}

void fireLazer () {
  ir.fireType = 0; // Normal gun fire shot
  Serial.println ( "Calling fireAll" );
  ir.fireAll ();
}

void explode() {
  digitalWrite (6,1);
  delay (400);
  digitalWrite (6,0);
}
int lastButton = 1;
void loop () {
  int value = digitalRead (5);
  if (value != lastButton) {
    lastButton = value;
    if (value == 0)
    {
       Serial.println ( "button pushed" );
       fireTimeout = millis() + 1000;
       ceaseFire = millis() + 6000;  // Fire for six seconds
       explode();
    }   
  }
  
  if (fireTimeout > 0) {
    if (millis() > fireTimeout) {
      fireLazer();      
      fireTimeout = millis() + 400;
    }
    if (millis() > ceaseFire) {
      ceaseFire = 0;
      fireTimeout = 0;
    }
  }
}
