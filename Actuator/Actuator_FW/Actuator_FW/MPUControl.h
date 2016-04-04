// MPUControl.h

#ifndef _MPUCONTROL_h
#define _MPUCONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/*#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>*/

class MPUControl
{
	//MPU6050 mpu;

public:
	MPUControl();

};

#endif

