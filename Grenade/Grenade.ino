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
boolean irEnable = true;
unsigned long irTimeout = 0;

void setup() {
  ir.init(); 
  
  pinMode (5,INPUT);
  pinMode (3,OUTPUT);
  pinMode (6,OUTPUT);
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
  delay (500);
  digitalWrite (6,0);
  fireTimeout = millis() + 4200;
}
int lastButton = 1;
void loop () {
  int value = digitalRead (5);
  if (value != lastButton) {
    lastButton = value;
    if (value == 0)
    {
       Serial.println ( "button pushed" );
       explode();
    }   
    delay (100);
  }
  
  if (fireTimeout > 0) {
    if (millis() > fireTimeout) {
      fireTimeout = 0;
      fireLazer();
    }
  }
}
