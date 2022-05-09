#include "PumpStepper.h"

//#define _TYPE PumpStepper::SYRINGE
//#define _H (float) 47.75
//#define _V (float) 3.0

#define _H (float) 69.0
#define _V (float) 20.0

#define _INTERFACE AccelStepper::HALF4WIRE
#define _PIN1 48
#define _PIN2 49
#define _PIN3 30
#define _PIN4 31
#define _STOP_F A0
#define _STOP_B A1
#define _ENABLE true

PumpStepper ps(	PumpStepper::SYRINGE,
						_H,
						_V,
						_INTERFACE,
						_PIN1,
						_PIN2,
						_PIN3,
						_PIN4,
						_STOP_F,
						_STOP_B,
						_ENABLE);

void setup() {
	delay(500);
	//ps.calibratePumpMaxPosition();
	Serial.begin(230400);
	Serial.println("Pump setup");
//	ps.setVolumeTime(3.6,10.0);
}

void loop() {
	ps.setVolumeTime(-20,10);
	while(ps.runSpeedToPositionToStop());
	ps.setVolumeTime(5,10);
	while(ps.runSpeedToPositionToStop());
	ps.setVolumeTime(5,20);
	while(ps.runSpeedToPositionToStop());
	ps.setVolumeTime(10,20);
	while(ps.runSpeedToPositionToStop());
	Serial.println("Pump done");
	//while(true);
}
