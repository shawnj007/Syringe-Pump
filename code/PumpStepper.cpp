#include "PumpStepper.h"

PumpStepper::PumpStepper(	uint8_t interface,
							uint8_t pin1,
							uint8_t pin2,
							uint8_t pin3,
							uint8_t pin4,
							bool enable) {

	_as = AccelStepper(interface, pin1, pin2, pin3, pin4, enable);
	
	uint8_t type = PumpType::CONTINUOUS;
	float   h = 52;
	float	v = 3;
	
	_type = type;
		
	_steps_per_millimeter = CONTINUOUS_STEPS_PER_MILLIMETER;

	_steps_per_millimeter *= (interface == AccelStepper::FULL4WIRE ? 0.5 : 1.0);
	
	_h  = h;
	_v  = v;
	_a1 = _h/_v; // units of mm / mL
	
	_steps_per_milliliter = _steps_per_millimeter * _a1;
	
	_max_position = 0;
	_max_rate = CONTINUOUS_MAX_RATE;
	
	_target_position = 0;
	
}

PumpStepper::PumpStepper(	uint8_t type,
							float   h,
							float   v,
							uint8_t interface,
							uint8_t pin1,
							uint8_t pin2,
							uint8_t pin3,
							uint8_t pin4,
							uint8_t stop_f,
							uint8_t stop_b,
							bool enable) {
	
	/*
	Serial.print(type);
	Serial.print(" ");
	Serial.print(h);
	Serial.print(" ");
	Serial.print(v);
	Serial.print(" ");
	Serial.print(interface);
	Serial.println(" ");
	*/
	
	_as = AccelStepper(interface, pin1, pin2, pin3, pin4, enable);

	_type = type;
	
	_steps_per_millimeter = SYRINGE_STEPS_PER_MILLIMETER;

	_steps_per_millimeter *= (interface == AccelStepper::FULL4WIRE ? 0.5 : 1.0);
	
	_h  = h;
	_v  = v;
	_a1 = _h/_v; // units of mm / mL
	
	_steps_per_milliliter = _steps_per_millimeter * _a1;
	
	/*
	Serial.print(" _steps_per_millimeter ");
	Serial.print(_steps_per_millimeter);
	Serial.print(" _h ");
	Serial.print(_h);
	Serial.print(" _v ");
	Serial.print(_v);
	Serial.print(" _a1 ");
	Serial.print(_a1);
	
	Serial.print(" steps_per_milliliter ");
	Serial.println(_steps_per_milliliter);
	*/
	
	_max_position = h * _steps_per_millimeter;
	_max_rate = SYRINGE_MAX_RATE;
	
	_target_position = 0;
	
	pinMode(stop_f, INPUT_PULLUP);
	pinMode(stop_b, INPUT_PULLUP);
	
	_pin_stop[DIRECTION_FORWARD]  = stop_f;
	_pin_stop[DIRECTION_BACKWARD] = stop_b;
}

void PumpStepper::setMaxRate(float rate) { // rate in mL / seconds
	float speed = _steps_per_millimeter * _a1 * rate; // steps / second
	//Serial.print("speed ");
	//Serial.println(speed);
	_as.setMaxSpeed(speed);
	_as.setSpeed(speed);
}

void PumpStepper::setVolumeTime(float milliliters, float seconds) {
	/*
	Serial.print(" milliliters ");
	Serial.print(milliliters);
	Serial.print(" seconds ");
	Serial.print(seconds);
	
	Serial.print(" steps_per_milliliter ");
	Serial.print((long) _steps_per_milliliter);
	*/
	
	long relative_position = (long) (_steps_per_milliliter * milliliters);
	
	/*
	Serial.print(" relative_position ");
	Serial.print(relative_position);
	
	Serial.print(" _as.currentPosition ");
	Serial.print(_as.currentPosition() );
	*/
	
	_target_position = _as.currentPosition() + relative_position;

	/*
	Serial.print(" mL/sec & steps _target_position ");
	Serial.println(_target_position);
	*/

	float rate = milliliters / seconds;
	_direction = (rate >= 0 ? DIRECTION_FORWARD : DIRECTION_BACKWARD );

	/*
	Serial.print(" rate ");
	Serial.print(rate);
	*/
	
	_as.moveTo(_target_position);
	setMaxRate(rate);
}

long PumpStepper::currentPosition() {
	return _as.currentPosition();
}

bool PumpStepper::check_stop() {
	long currentPosition = _as.currentPosition();
	/*
	Serial.print((_direction == DIRECTION_FORWARD ? "FORWARD" : "BACKWARD"));
	Serial.print(" currentPosition ");
	Serial.print(currentPosition);
	Serial.print(" _target_position ");
	Serial.print(_target_position);
	Serial.println();
	*/
	if ((_direction == DIRECTION_FORWARD  && currentPosition >= _target_position)
	 || (_direction == DIRECTION_BACKWARD && currentPosition <= _target_position)) {
	 	return false;
	}
	//Serial.print(" ");
	//Serial.println(digitalRead(_pin_stop[_direction]));
	return true; // ((_type == PumpType::CONTINUOUS) || (digitalRead(_pin_stop[_direction])));  // 0 is closed (grounded) circuit, stop in this direction, 1 is open, keep running
}

void PumpStepper::stop() {
	_as.stop();
	if (_type == PumpType::SYRINGE)    _as.setCurrentPosition(_target_position);
	if (_type == PumpType::CONTINUOUS) _as.setCurrentPosition(0);
	_as.disableOutputs();
}

bool PumpStepper::runToStop() {
	// Check for endstop signal
	if (check_stop()) {
		_as.run();
		return true;
	} else {
		if (_as.isRunning()) stop();
		return false;
	}
}

bool PumpStepper::runSpeedToStop() {
	// Check for endstop signal
	if (check_stop()) {
		_as.runSpeed();
		return true;
	} else {
		if (_as.isRunning()) stop();
		return false;
	}
}

bool PumpStepper::runSpeedToPositionToStop() {
	
	// Check for endstop signal
	if (check_stop()) {
		_as.runSpeedToPosition();
		return true;
	} else {
		if (_as.isRunning()) stop();
		return false;
	}
}

long PumpStepper::setMaxPosition(long max_position) {
	_max_position = max_position; //MIN(70.0 * _steps_per_millimeter, max_position);
	return _max_position;
}

float PumpStepper::getTimeToDispense(float volume) {
	//Serial.print("V ");
	//Serial.print(volume);
	//Serial.println(" mL ");
	float d = volume * _a1; // mm = mL * (mm/mL)
	//Serial.print("d ");
	//Serial.print(d);
	//Serial.println(" mm ");
	float t = d / _max_rate; // sec = mm / (mm/sec)
	//Serial.print("t ");
	//Serial.print(t);
	//Serial.println(" secs");
	return t;
} 

float PumpStepper::getDispensedDistance() {
	long currentPosition = _as.currentPosition();
	return (float) currentPosition / _steps_per_millimeter;
}

float PumpStepper::getDispensedVolume() {
	long currentPosition = _as.currentPosition();
	return (float) currentPosition / _steps_per_milliliter;
}

long PumpStepper::calibrateSyringePump() {
	if (_type == PumpType::SYRINGE) {
		// Count revolutions to:
		
		// Run pump to endstop
		_as.moveTo(SYRINGE_MAX_POSITION);
		_as.setSpeed(100);
		while (runSpeedToStop());
		
		long max_pos = 0;
		
		// Run pump to other endstop
		_as.moveTo(-(SYRINGE_MAX_POSITION));
		_as.setSpeed(-100);
		while (runSpeedToStop());

		// Set the calibrated position of end stop
		return max_pos;
	}
	
	return 0;
}

long PumpStepper::calibrateSyringePumpMaxPosition() {
	return setMaxPosition(calibrateSyringePump());
}
