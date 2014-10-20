#include "IRLazer.h"


#define SHOTPULSES 41
PROGMEM prog_uint16_t shotPulses[]    = {1812,1728,1808,1724,1812,1740,1812,1724,1812,1724,
                                        1812,1732,1812,1724,1808,1728,1816,1728,1824,1712,
                                        1812,1724,1812,1740,1804,1732,1804,1728,1816,1732,
                                        1812,1724,1812,1724,1812,1732,1808,1728,1816,1720,1816};
                                        
#define RESETPULSES 29
PROGMEM prog_uint16_t resetPulses[]  = {2896,2872,2896,2860,2880,2884,2888,2856,2888,2864,
                                        2888,2876,2880,2864,2884,2876,2876,2888,2888,2876,
                                        2896,2868,2884,2884,2892,2880,2888,2876,2892};
                                        
void IRLazer::init (void)
{
  pinMode (irTransmit,OUTPUT); 
  digitalWrite (irTransmit, 0); // Keep output low when not in use
}

int IRLazer::maxFireTypes (void)
{
  return 1;
} 

// return non-zero if a shot was detected
int IRLazer::IRDetected ( void )
{
  int shot = 0;
  unsigned int val; // IR value
  if (irReady)
  {
    val = decodeBits (7, true, false);    
    if (val == 0xFFFF) // Shot pulse
      shot = 1;
    else 
    {
      val = decodeBits ( 10, true, false);
      if (val == 0xFFFF) // Heal pulse
        shot = 2;
      else
        val = decodeBits ( 38, true, false); // To display the shot, set last parameter = true
    }  
    resetIR(); 
  }    
  return shot;
}

/* Leave pin off for time (given in microseconds) */
void space(int time) {
  // Sends an IR space for the specified number of microseconds.
  // A space is no output, so the PWM output is disabled.
  TCCR2A &= ~(_BV(COM2B1)); // Disable pin 3 PWM output
  delayMicroseconds(time * 50);
}

void sendRaw(unsigned int uDelay, int index)
{
  // Toggle between transmitting data, and doing nothing
  if (index & 1)
    TCCR2A &= ~(_BV(COM2B1)); // Disable pin 3 PWM output
  else 
    TCCR2A |= _BV(COM2B1); // Enable pin 3 PWM output
    
  delayMicroseconds(uDelay);
}

void enableIROut() 
{
  #define SYSCLOCK 16000000  // main Arduino clock
  int khz = 38;

  // Enables IR output.  The khz value controls the modulation frequency in kilohertz.
  // The IR output will be on pin 3 (OC2B).
  // This routine is designed for 36-40KHz; if you use it for other values, it's up to you
  // to make sure it gives reasonable results.  (Watch out for overflow / underflow / rounding.)
  // TIMER2 is used in phase-correct PWM mode, with OCR2A controlling the frequency and OCR2B
  // controlling the duty cycle.
  // There is no prescaling, so the output frequency is 16MHz / (2 * OCR2A)
  // To turn the output on and off, we leave the PWM running, but connect and disconnect the output pin.
  // A few hours staring at the ATmega documentation and this will all make sense.
  // See my Secrets of Arduino PWM at http://arcfn.com/2009/07/secrets-of-arduino-pwm.html for details.

  
  // Disable the Timer2 Interrupt (which is used for receiving IR)
  // TIMSK2 &= ~_BV(TOIE2); //Timer2 Overflow Interrupt
      // atmega168
  // Not sure about next line
  //TIMSK1 &= ~_BV(OCIE1A); // Disable the interrupt that is used to read RF

  pinMode(3, OUTPUT);
  digitalWrite(3, LOW); // When not sending PWM, we want it low
  
  // COM2A = 00: disconnect OC2A
  // COM2B = 00: disconnect OC2B; to send signal set to 10: OC2B non-inverted
  // WGM2 = 101: phase-correct PWM with OCRA as top
  // CS2 = 000: no prescaling
  TCCR2A = _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS20);

  // The top value for the timer.  The modulation frequency will be SYSCLOCK / 2 / OCR2A.
  OCR2A = SYSCLOCK / 2 / khz / 1000;
  OCR2B = OCR2A / 2;  // 50% duty cycle... / 3; =  33% duty cycle
}

void IRLazer::sendFirePulse(void)
{
  unsigned int myDelay;
  
  enableIROut();
  if (fireType == 0)
  {
     for (int j = 0; j < SHOTPULSES; j++) 
     {
       myDelay = pgm_read_word_near( shotPulses + j);
       sendRaw (myDelay, j);
     }
     space(0); // Just to be sure
  }
  else if (fireType == 1)
  {
     for (int j = 0; j < RESETPULSES; j++) 
     {
       myDelay = pgm_read_word_near( resetPulses + j);
       sendRaw (myDelay, j);
     }
     space(0); // Just to be sure
  }
  

}

/* 
   Limitation: 
      You cannot place any Serial.println instructions here.
	  The program will not run 
*/
void IRLazer::fireAll (void)
{
  Timer1->stop();
  sendFirePulse(); // Update fireTimeout and send pulse
  Timer1->resume();   
}
