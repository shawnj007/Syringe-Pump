
#ifndef PumpStepper_h
#define PumpStepper_h

#include <AccelStepper.h>
//#include <MultiStepper.h>

// 3 milliliter
//#define V ((float)  3.0)                 // mL
//#define H ((float) 46.5)                 // mm

// 20 milliliter
//#define H  ((float)  68.5)                // mm
//#define V  ((float)  20.0)                // mL

//#define A  ((float) (V/H))                // mL / mm
//#define A1 ((float) (H/V))                // mm / mL
//#define MILLIMETER_PER_MILLILITER A1

// CONTINUOUS Pump characteristics
#define CONTINUOUS_VOL_RATIO ((float) (2.0 / 3.0))					// Losses to "Squeeze" sections in pump

#define CONTINUOUS_VOL_R ((float) 16.20)							//  16.200 mm - radius of tube in pump
#define CONTINUOUS_VOL_C ((float) 2.0 * 3.14159 * CONTINUOUS_VOL_R)	// 101.788 mm - circumfrance of tube in pump

#define CONTINUOUS_R_H ((float) (1.0/CONTINUOUS_VOL_C))     		// rev / mm
#define CONTINUOUS_HALFSTEP_RATIO ((float) (400.0))     			// steps / rev
#define CONTINUOUS_STEPS_PER_MILLIMETER ( CONTINUOUS_VOL_RATIO * CONTINUOUS_HALFSTEP_RATIO * CONTINUOUS_R_H )
#define CONTINUOUS_MAX_RATE ((float) 10.0)

// SYRINGE Pump characteristics
#define SYRINGE_R_H ((float) (1.0/0.5))          					// rev / mm
#define SYRINGE_GEAR_RATIO ((float) (9.0/22.0))  					// REV / rev
#define SYRINGE_HALFSTEP_RATIO ((float) (400.0))     				// steps / REV
#define SYRINGE_STEPS_PER_MILLIMETER ( SYRINGE_HALFSTEP_RATIO * SYRINGE_GEAR_RATIO * SYRINGE_R_H )

//		200 × (9÷22) ÷ .5             =  163.63 steps / mm

//		200 × (9÷22) ÷ .5 x 68.5 ÷ 20 =  560.45 steps / mL for 20mL syringe ~ 11,200 pulses
//		200 × (9÷22) ÷ .5 x 46.3 ÷  3 = 2525.45 steps / mL for  3mL syringe ~  7,600 pulses

#define SYRINGE_MAX_RATE ((float) 5.0) // in mm / sec
#define SYRINGE_MAX_DISTANCE ((float) 70.0) // mm	
#define SYRINGE_MAX_POSITION (SYRINGE_STEPS_PER_MILLIMETER * SYRINGE_MAX_DISTANCE) // steps ~ 11455

//		(  500 steps / second ) / (  560 steps / mL ) = 0.89 mL / seconds
//		(  500 steps / second ) / ( 2525 steps / mL ) = 0.20 mL / seconds

//		( 1000 steps / second ) / (  560 steps / mL ) = 1.79 mL / seconds
//		( 1000 steps / second ) / ( 2525 steps / mL ) = 0.40 mL / seconds

//		( 3000 steps / second ) / (  560 steps / mL ) = 5.35 mL / seconds
//		( 3000 steps / second ) / ( 2525 steps / mL ) = 1.19 mL / seconds

#define SYRINGE_STEPS_PER_MILLILITER (SYRINGE_STEPS_PER_MILLIMETER * SYRINGE_MILLIMETER_PER_MILLILITER)

#define MIN(A,B) (A > B ? B : A )
#define MAX(A,B) (A < B ? B : A )

class PumpStepper : public AccelStepper
{
public:

	typedef enum
	{
		CONTINUOUS = 0,
		SYRINGE    = 1
	} PumpType;
	
	typedef enum
    {
        DIRECTION_FORWARD  = 0,   ///< Counter-Clockwise
	    DIRECTION_BACKWARD = 1    ///< Clockwise
    } Direction;
	
	PumpStepper(uint8_t interface = AccelStepper::HALF4WIRE,
				uint8_t pin1 = 2,
				uint8_t pin2 = 3,
				uint8_t pin3 = 4,
				uint8_t pin4 = 5,
				bool enable = true);
	
	PumpStepper(uint8_t type,
				float   h,
				float   v,
				uint8_t interface = AccelStepper::HALF4WIRE,
				uint8_t pin1 = 2,
				uint8_t pin2 = 3,
				uint8_t pin3 = 4,
				uint8_t pin4 = 5,
				uint8_t stop_f = 6,
				uint8_t stop_b = 7,
				bool enable = true);
	
	void setMaxRate(float rate);
	void setVolumeTime(float milliliters, float seconds);

	long currentPosition();
	
	void stop();
	bool check_stop();
	bool runToStop();
	bool runSpeedToStop();
	bool runSpeedToPositionToStop();
	
	float getTimeToDispense(float volume);
	float getDispensedDistance();
	float getDispensedVolume();
	
	long setMaxPosition(long max_position);
	long calibrateSyringePump();
	long calibrateSyringePumpMaxPosition();

private:
	AccelStepper _as;

	uint8_t		_type;

	float		_h;
	float		_v;
	float		_a1;
	float		_max_rate;
	float		_steps_per_milliliter;
	float		_steps_per_millimeter;
	
	Direction	_direction;
	
	long		_max_position;
	long		_target_position;
	
    uint8_t     _pin_stop[2];
	
};

#endif
