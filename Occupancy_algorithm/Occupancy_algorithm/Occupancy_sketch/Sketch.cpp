/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

#include <Adafruit_GPS.h>
#include <hcsr04.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <Adafruit_AMG88xx.h>
#include "TFT_screen.h"
#include <avr/sleep.h>
//#include "Thermal_camera.h"
//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio


//#define Telecom_TX  //connects to rx pin on telecom module // User hardware serial 1
//#define Telecom_RX  //connects to tx pin on telecom module
#define Power_pin 23//connects to power pin on telecom module
#define TRIG_PIN 29
#define ECHO_PIN 27
#define PIR_read_pin 31
//#define Telecom_enable_pin 49
#define Ping_PIR_enable_pin 33
#define Camera_enable_pin 35

HCSR04 hcsr04(TRIG_PIN, ECHO_PIN, 20, 4000); //setup ping sensor

struct Thermal_SizeTemp_Struct Thermal_size_temp_read;

int Distance;
int Detection_flag;
int Sleep_time = 290000; //4:50 minutes of sleep time

enum states {
	Idle,
	Car_Running,
	Occupant_no_danger,
	Occupant_1,
	Occupant_2,
	Occupant_3,
};

states State = Idle;

void setup() {
	// put your setup code here, to run once:

	//Serial.begin(9600);
	//Serial.println(F("AMG88xx thermal camera!"));

	pinMode(PIR_read_pin, INPUT);
	
	Serial.begin(9600);
	Serial.println(F("AMG88xx thermal camera!"));

	Thermal_setup();

}

void loop() {
	// put your main code here, to run repeatedly:

	//determine phase, the functions below should be placed in a state machine
	
	Thermal_size_temp_read = Thermal_read(); //the values being returned here need to be determined in testing //returns max temp detected and any number of pixels above 25C -3/7 KF

	Distance = hcsr04.distanceInMillimeters();

	digitalRead(PIR_read_pin);

	switch (State)
	{
		Idle:
		if ((Thermal_size_temp_read.high_temp = MAXTEMP) || (Thermal_size_temp_read.size > 5)){
			Detection_flag = true; 
			//check for temp here
			State = Occupant_no_danger;
		}
		break;
		
		Occupant_no_danger:
		Default:
		State = Idle;
		break;
	}

	/*
	sleep here with delay 
	*/
	
}