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
#include "IR_cam.h"
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
#define Occupant_thickness 170 //based on Kyron's waist

struct Thermal_SizeTemp_Struct Thermal_parsed;

int Distance;
int Seat_Distance = 1000; //to be changed per car based on calibration
int PIR;
int Seat_load;
int IR;
int Detection_flag = false;
int Sleep_time = 290000; //4:50 minutes of sleep time

enum states {
	Running, //accelerometer detects movement
	Stopped, //accelerometer detects no movement and no driver detected, determine if state machine needed or go to sleep
	CO1, // 70 ppm
	CO2, // 150 ppm
	TH0, // No threat
	TH1, // 80 degrees, send notification
	TH2, // 86 degrees, lower windows
	TH3, // 91 degrees, call 911
	TH4, // 105 degrees, Open door
};

states State = Stopped;

int Occupant_detect(){
	Distance = hcsr04.distanceInMillimeters();
	PIR = digitalRead(PIR_read_pin);
	Seat_load = 1; // hard coded for positive reading, to be backed by logic when module is designed
	Thermal_parsed = Thermal_read(); 
	
	int Object;
	int IR;
	
	int Object_size = Seat_Distance - Distance;	
	
	switch (State) //Need to add in CO states
	{
		case Running: //Under 80 deg F
		case Stopped:
		case TH0:
			if ((Object_size > Occupant_thickness) || (Seat_load == 1)){
				Object = 1;
			} 
			if ((PIR == 1)||(Thermal_parsed.detected == 1)){
				IR = 1;	
			}
			if ((Object == 1)&&(IR == 1)){
				return 1;
			} else {
				return 0;
			}
		break;
		
		case TH1: //Above 80 deg F
		case TH2:
		case TH3:
		case TH4:
			if ((Object_size > Occupant_thickness) && (Seat_load == 1)){
				Object = 1;
			}
			if ((PIR == 1)||(Thermal_parsed.detected == 1)){
				IR = 1;
			}
			if ((Object == 1)||(IR == 1)){
				return 1;
				} else {
				return 0;
			} 
		break;
		
		default:
		break;
	}	
}

void setup() {
	// put your setup code here, to run once:

	//Serial.begin(9600);
	//Serial.println(F("AMG88xx thermal camera!"));

	pinMode(PIR_read_pin, INPUT);
	
	Serial.begin(9600);
	Serial.println(F("AMG88xx thermal camera!"));

	Thermal_setup();

}

//int Print_delay = 0;

void loop() {
	// put your main code here, to run repeatedly:

	//Print_delay++;
	//determine phase, the functions below should be placed in a state machine
	
	//Thermal_size_temp_read = Thermal_read(); //the values being returned here need to be determined in testing //returns max temp detected and any number of pixels above 25C -3/7 KF
//
	//Distance = hcsr04.distanceInMillimeters();
	//
	//if(Print_delay >= 10){
		//Print_delay = 0;
		//Serial.print("Ping distance: ");
		//Serial.println(Distance);
//
		////digitalRead(PIR_read_pin);
		//Serial.print("PIR read: ");
		//Serial.println(digitalRead(PIR_read_pin));
		//Serial.print("Max Temp Detected: "); Serial.print(Thermal_size_temp_read.max_temp); Serial.print("\n");
		//Serial.print("Number of pixels above "); Serial.print(DETECTEMP ); Serial.print(": " ); Serial.print(Thermal_size_temp_read.size, DEC); Serial.print("\n\n");
	//}

	switch (State)
	{
		case Running:
		
		break;
		
		case Stopped:
		
		break;
		
		default:
		State = Running;
		break;
	}

	/*
	sleep here with delay
	*/
	//delay(500);
	
}