#include <TimerOne.h>
#include <IR.h>
#include <IRTank.h>

#define IRRECEIVEPIN 10
#define IRTRANSMIT 3
#define REPAIRROBOT false
#define REDLED 12
#define GREENLED 11
#define CANNONBUTTON 9
#define GUNBUTTON 8

IRTank ir (IRRECEIVEPIN, IRTRANSMIT, &Timer1, REPAIRROBOT);
unsigned long timeout = 0;
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
  timeout = millis() + 750;
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

/*
   Simulator the firing of the main cannon of the huan qi battle tank
   void IRTank::sendFirePulse(void) will be called 
*/
void fireHuanQi (int fireType) {
  ir.fireType = fireType + 1; // Huan Qi Cannon = FireType 4, Gun = 6 
  ir.fireAll ();
  Serial.print ( "fireType: " );
  Serial.println ( ir.fireType );
  digitalWrite (GREENLED,0);
}

int lastCannon = 0;
int lastGun = 0;
void loop () {
  int value = digitalRead (CANNONBUTTON);
  if (value != lastCannon)
  {
    lastCannon = value;
    if (!value) {
       Serial.println ( "Fire!" );
       delay (300);
    }
  }
  
  value = digitalRead (GUNBUTTON);
  if (value != lastGun)
  {
    lastGun = value;
    if (!value) {
       Serial.println ( "Fire!" );
       delay (300);
    }
  }

  int irDetected = ir.decodeBits (33, true, false);
    
  static int redGreen = 0;
  
  if (timeout > 0) {
    if (millis() > timeout) {
       timeout = 0;
       digitalWrite (REDLED,0);
       digitalWrite (GREENLED,0);
       fireTimeout = 1; // Set to non-zero so code will trigger below
       digitalWrite (GREENLED,1);
    }
  }
  
  if (fireTimeout) 
    if (millis() > fireTimeout) {
        // Allow time for each individual fire sequence.
        //fireHuanQi (4);
        fireTimeout = 0; // Done
        //Serial.print ( "Done" );
      }  
    
  
  if (irDetected ==  -1) {
     Serial.println ( "Got something" );
     redGreen = 1 - redGreen;
     if (redGreen == 0)
     {
       digitalWrite (REDLED,1);
       digitalWrite (GREENLED,0);
     }
     else 
     {
       digitalWrite (REDLED,0);
       digitalWrite (GREENLED,1);       
     }
     irEnable = false;
     ir.resetIR(); 
     delay (1000);
     irEnable = true;  
  }
  else if (millis() > irTimeout) {
    ir.resetIR();
    irTimeout = millis() + 1000;
  }
}

