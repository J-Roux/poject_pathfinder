#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <I2Cdev.h>
#include "MPU6050_6Axis_MotionApps20.h"

//#define DEBUG // comment for disable debug messanges

#ifdef DEBUG
#define DEBUG_PRINT(messange) Serial.print(messange)
#define DEBUG_PRINTLN(messange) Serial.println(messange)
#else 
#define DEBUG_PRINT(messange) 
#define DEBUG_PRINTLN(messange)
#endif

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
MPU6050 mpu;

// MPU control/status vars
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

						// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

const float declinationAngle = 0.1338;

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
//volatile uint_fast32_t counter = 0;
void dmpDataReady() {
	mpuInterrupt = true;
	//counter++;
}

bool compassInit(Adafruit_HMC5883_Unified * mag)
{
	if (!mag->begin())
	{
		Serial.println("HMC5883 is not detected or something trouble with init compass.");
		return false;
	}
	return true;
}

bool MPUInit(MPU6050 * mpu)
{
	mpu->initialize();
	if (!mpu->testConnection())
	{
		Serial.println("MPU6050 is not detected or something trouble with init module.");
		return false;
	}
	DEBUG_PRINTLN("MPU init");

	uint16_t devStatus = mpu->dmpInitialize();

	if (!devStatus)
	{
		// supply your own gyro offsets here, scaled for min sensitivity
		mpu->setXGyroOffset(220);
		mpu->setYGyroOffset(76);
		mpu->setZGyroOffset(-85);

		// turn on the DMP, now that it's ready
		DEBUG_PRINTLN("Enabling DMP...");
		mpu->setDMPEnabled(true);

		// enable Arduino interrupt detection
		DEBUG_PRINTLN("Enabling interrupt detection");
		attachInterrupt(0, dmpDataReady, RISING);
		mpuIntStatus = mpu->getIntStatus();

		// set our DMP Ready flag so the main loop() function knows it's okay to use it
		DEBUG_PRINTLN("DMP ready!");

		// get expected DMP packet size for later comparison
		packetSize = mpu->dmpGetFIFOPacketSize();
	}
	else 
	{
		// ERROR!
		// 1 = initial memory load failed
		// 2 = DMP configuration updates failed
		// (if it's going to break, usually the code will be 1)
		Serial.print(F("DMP Initialization failed (code "));
		Serial.print(devStatus);
		Serial.println(F(")"));
		return false;
	}
	return true;
}

void setup(void)
{
	Serial.begin(115200);
	while (!Serial);

	Wire.begin();

	Serial.println("Initializing I2C devices...");

	if (!compassInit(&mag) || !MPUInit(&mpu))
	{
		Serial.println("Error while initialization! End working...");
		while (true);
	}
	Serial.println("Initializing complete.\n");

	delayMicroseconds(4900); // first interrupt
}

uint_fast32_t ticks = 0;

void loop(void)
{

	// wait for MPU interrupt or extra packet(s) available
	while (!mpuInterrupt && fifoCount < packetSize);

	//ticks = micros();

	// reset interrupt flag and get INT_STATUS byte
	mpuInterrupt = false;
	mpuIntStatus = mpu.getIntStatus();

	// get current FIFO count
	fifoCount = mpu.getFIFOCount();

	// check for overflow (this should never happen unless our code is too inefficient)
	if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
		// reset so we can continue cleanly
		mpu.resetFIFO();
		//Serial.println(F("FIFO overflow!"));
		return;
		// otherwise, check for DMP data ready interrupt (this should happen frequently)
	}
	else if (mpuIntStatus & 0x02) {
		// wait for correct available data length, should be a VERY short wait
		while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

		DEBUG_PRINT("Fifo count: ");
		DEBUG_PRINTLN(fifoCount);

		// read a packet from FIFO
		mpu.getFIFOBytes(fifoBuffer, packetSize);

		// track FIFO count here in case there is > 1 packet available
		// (this lets us immediately read more without waiting for an interrupt)
		fifoCount -= packetSize;

		
		// display Euler angles in degrees
		mpu.dmpGetQuaternion(&q, fifoBuffer);
		mpu.dmpGetGravity(&gravity, &q);
		mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
		Serial.print("ypr\t");
		Serial.print(ypr[0] * 180 / M_PI);
		Serial.print("\t");
		Serial.print(ypr[1] * 180 / M_PI);
		Serial.print("\t");
		Serial.println(ypr[2] * 180 / M_PI);

		// display real acceleration, adjusted to remove gravity
		mpu.dmpGetQuaternion(&q, fifoBuffer);
		mpu.dmpGetAccel(&aa, fifoBuffer);
		mpu.dmpGetGravity(&gravity, &q);
		mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
		mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
		Serial.print("aworld\t");
		Serial.print(aaWorld.x);
		Serial.print("\t");
		Serial.print(aaWorld.y);
		Serial.print("\t");
		Serial.println(aaWorld.z);

	}

	DEBUG_PRINT("Time for mpu: ");
	//DEBUG_PRINTLN(micros() - ticks);

	//ticks = micros();
	// magnitometer
	sensors_event_t event;
	mag.getEvent(&event);

	float heading = atan2(event.magnetic.y, event.magnetic.x);	
	heading += declinationAngle;

	if (heading < 0)
		heading += 2 * M_PI;

	if (heading > 2 * M_PI)
		heading -= 2 * M_PI;
	Serial.print("Heating: "); 
	Serial.println(heading * 180 / M_PI);
	DEBUG_PRINT("Time for mag: ");
	//DEBUG_PRINTLN(micros() - ticks);
}