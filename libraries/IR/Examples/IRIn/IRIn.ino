#include <TimerOne.h>
#include <IR.h>

IR IRReceiver (15);

void callback()
{
  IRReceiver.callback();
}

void setup()
{
  Serial.begin (115200);
  Timer1.initialize(50);         // initialize timer1, and set a 1/2 second period
  Timer1.attachInterrupt(callback,50);  
 
  Serial.println ( "Ready to receive an IR pulse" );
}

void handleCommand (int command )
{
  char ch = 0;
  switch (command) 
  {
    case 0: // Fire
      ch = 'F';
    break;
    case 1: // Up
      ch = 'U';
    break;
    case 2: // Dn
      ch = 'D';
    break;
    case 3: // Right
      ch = 'R';
    break;
    case 4: // Left
      ch = 'L';
    break;
  }
  if (ch)
  {
    Serial.print ("writing " );
    Serial.println ( ch );
    mySerial.write (ch);
  }  
}

int findPulse (int value) 
{
  int pulse = -1;
  switch (value )
  {
    case 0x1290:
      pulse = 0; // Fire
    break;
    case 0x1090:
      pulse = 1; // Up
    break;
    case 0x1890:
      pulse = 2; // Dn
    break;
    case 0x1490:
      pulse = 3; // Right
    break;
    case 0x1C90:
      pulse = 4; // Left
    break;  
    default:
      Serial.print ( "Could not find pulse: " );
      Serial.println ( value, HEX );
    break;
  }
  return pulse;
}

void loop()
{
  int val1;
 
  if (IRReceiver.irReady)
  {
    val1 = IRReceiver.decodeBits (15, true, true);    
    handleCommand (findPulse(val1));
    IRReceiver.resetIR(); 
  }  



}

