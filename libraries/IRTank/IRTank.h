#ifndef IR_Tank_h
#define IR_Tank_h
#include "IR.h"
#include <TimerOne.h>

class IRTank : public IR {
public:
  // repairRobotValue is set true if the unit is a repair robot
  IRTank ( int irrcvpin, int irTransmitPin, TimerOne * timer1, bool repairRobotValue) : IR (irrcvpin) {
    Timer1 = timer1;
	fireType = 0;
	irTransmit = irTransmitPin;
	repairRobot = repairRobotValue;
  }; // constructor

  // Methods 
  void init (void);
  void fireAll(void);
  void sendFirePulse(void);
  void fire(void) 
  {
    fireType = maxFireTypes();
	fireTimeout = 1; // Start firing pulses
  }
  int IRDetected();  
  
  // Fields
  int fireType;
  
private:
  int maxFireTypes (void); 
  TimerOne * Timer1;
  int irTransmit;
  unsigned long fireTimeout;
  bool repairRobot;
};
#endif