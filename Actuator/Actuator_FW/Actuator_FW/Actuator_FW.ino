#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include "I2Cdev.h"
#include "MPU6050.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

int16_t ax, ay, az;
int16_t gx, gy, gz;

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
MPU6050 accelgyro;

void setup(void)
{
	Serial.begin(38400);
	Wire.begin();

	Serial.println("Initializing I2C devices...");

	accelgyro.initialize();
	if (!accelgyro.testConnection())
	{
		Serial.println("MPU6050 connection failed");
		while (1);
	}	

	accelgyro.setXGyroOffset(220);
	accelgyro.setYGyroOffset(76);
	accelgyro.setZGyroOffset(-85);
	accelgyro.setZAccelOffset(1788); // 1688 factory default for my test chip

	if (!mag.begin())
	{
		/* There was a problem detecting the HMC5883 ... check your connections */
		Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
		while (1);
	}

}

void loop(void)
{
	/* Get a new sensor event */
	sensors_event_t event;
	mag.getEvent(&event);

	float heading = atan2(event.magnetic.y, event.magnetic.x);
	//float pitch = atan2(event.magnetic.y, event.magnetic.z);


	// Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
	// Find yours here: http://www.magnetic-declination.com/
	// Mine is: -13* 2' W, which is ~13 Degrees, or (which we need) 0.22 radians
	// If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
	float declinationAngle = 0.1338;
	heading += declinationAngle;
	//pitch += declinationAngle;

	// Correct for when signs are reversed.
	if (heading < 0)
		heading += 2 * PI;

	// Check for wrap due to addition of declination.
	if (heading > 2 * PI)
		heading -= 2 * PI;

	// Convert radians to degrees for readability.
	float headingDegrees = heading * 180 / M_PI;
	//float pitchDegrees = pitch * 180 / M_PI;

	Serial.print("Heating: "); 
	Serial.println(headingDegrees);
	//Serial.print(" ");
	//Serial.println(pitchDegrees);

	accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

	/*Serial.print("a/g:\t");
	Serial.print(ax); Serial.print("\t");
	Serial.print(ay); Serial.print("\t");
	Serial.print(az); Serial.print("\t");
	Serial.print("/");
	Serial.print(gx); Serial.print("\t");
	Serial.print(gy); Serial.print("\t");
	Serial.println(gz);*/

	float pitchDegrees = atan2(gy, gz) * 180 / M_PI;
	Serial.print("Pitch: "); 
	Serial.println(pitchDegrees);
		
	delay(1000);
}