#include <TimerOne.h>
#include <IR.h>
#include <IRLazer.h>
#include <MemoryFree.h>

#define IRRECEIVEPIN 10
#define IRTRANSMIT 3
#define REPAIRROBOT false
#define REDLED 12
#define GREENLED 11
#define CANNONBUTTON 9
#define GUNBUTTON 8

IRLazer ir (IRRECEIVEPIN, IRTRANSMIT, &Timer1, REPAIRROBOT);
unsigned long redTimeout = 0;
unsigned long greenTimeout = 0;
unsigned long fireTimeout = 0;
boolean irEnable = true;
unsigned long irTimeout = 0;

void setup()
{
  ir.init(); 
  Timer1.initialize (25);
  Timer1.attachInterrupt(callback,25);  
  
  pinMode (REDLED,OUTPUT);
  pinMode (GREENLED,OUTPUT);
  
  // Button for Main Cannon
  pinMode (CANNONBUTTON,INPUT);
  digitalWrite (CANNONBUTTON,1); // Internal pull up set
  
  // Button for Machine Gun
  pinMode (GUNBUTTON,INPUT);
  digitalWrite (GUNBUTTON,1); // Internal pull up set
  
  Serial.begin (115200);
  Serial.println ( "Ready" );
  digitalWrite (REDLED,1);
  digitalWrite (GREENLED,1);
  redTimeout = millis() + 1000;
  greenTimeout = millis() + 1000;
  
  Serial.print ( "Free Memory: " );
  Serial.println ( freeMemory() );
}

void callback() // Timer1 is set to 25 microsecond to balance PWM output
{
  static bool skip = false;
  if (irEnable) {
    skip = !skip;
    if (!skip)
      ir.callback();
  }  
}

void fireLazer (int which) {
  irEnable = false;
  ir.fireType = which; // 0 = normal pulse, 1 = heal
  ir.fireAll ();
  digitalWrite (GREENLED,1);
  greenTimeout = millis() + 500;
  irEnable = true;
}

int lastCannon = 0;
int lastGun = 0;
void loop () {
  int value = digitalRead (CANNONBUTTON);
  if (value != lastCannon)
  {
    lastCannon = value;
    if (!value) {
       fireLazer (0);
       Serial.println ( "Fire lazer gun" );
       delay (300);
    }
  }
  
  value = digitalRead (GUNBUTTON);
  if (value != lastGun)
  {
    lastGun = value;
    if (!value) {
       fireLazer (1);
       Serial.println ( "Heal" );
       delay (300);
    }
  }

  int irDetected = ir.decodeBits (33, true, false);
    
  static int redGreen = 0;
  
  if (redTimeout > 0)
     if (millis() > redTimeout) {
        digitalWrite (REDLED, 0);
        redTimeout = 0;
     }  
    
  if (greenTimeout > 0) 
     if (millis() > greenTimeout) {
       digitalWrite (GREENLED, 0);
       greenTimeout = 0;
     }  
  
  if (irDetected ==  -1) {
     Serial.println ( "Got something" );
     digitalWrite (REDLED, 1);
     redTimeout = millis() + 500;
     irEnable = false;
     ir.resetIR(); 
     irEnable = true;  
  }
  else if (millis() > irTimeout) {
    ir.resetIR();
    irTimeout = millis() + 1000;
  }
}


