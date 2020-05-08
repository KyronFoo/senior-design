/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

//#include <Adafruit_GPS.h>
#include <hcsr04.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <Adafruit_AMG88xx.h>
#include "IR_cam.h"
#include <avr/sleep.h>
#include "Accelerometer.h"
#include "Temp_Humidity.h"
#include <string.h>
#include <Stream.h>
#include <stdio.h>
#include <stdlib.h>

#define Power_pin 23//connects to power pin on telecom module
#define TRIG_PIN 29
#define ECHO_PIN 27
#define PIR_read_pin 31
#define CO_read_pin A3
//#define Telecom_enable_pin 49
#define Ping_enable_pin 39
#define PIR_enable_pin 34
#define Camera_enable_pin 35
#define Alarm_Lights_pin 41
#define Car_unlock_pin 43
#define Car_door_open_pin 45
#define GPS_enable_pin 37

HCSR04 hcsr04(TRIG_PIN, ECHO_PIN, 20, 4000); //setup ping sensor
#define Occupant_thickness 170 //based on Kyron's waist

struct Thermal_SizeTemp_Struct Thermal_parsed;
MPU_data MPU_read;

struct occupancy_data {
	int Distance;
	int Object_size;
	bool PIR;
	bool Seat_load;
	bool IR;
};

int Seat_Distance = 1000; //to be changed per car based on calibration
int Detection_flag = false;
int Sleep_time = 290000; //4:50 minutes of sleep time
bool User_dismiss = 0; //user override flag
bool Deescalation_dismiss = 0; //software action override flag when temperatures are lowering as opposed to rising

bool Testing_mode = 1; //set this to 1 to enable testing mode, where all data is inputted and outputted via the serial port.

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

states State = Running;

bool Occupant_flag = false; //This flag is used both in occupant detect and in the main loop
occupancy_data current;
occupancy_data previous;

bool Occupant_detect(){
	if (!Testing_mode){
		current.Distance = hcsr04.distanceInMillimeters();
		current.PIR = digitalRead(PIR_read_pin);
		current.Seat_load = 1; // hard coded for positive reading, to be backed by logic when module is designed
		Thermal_parsed = Thermal_read();
		current.IR = Thermal_parsed.detected;
		} else {
		Serial.println("Occupant_detection");
		return 0;
		Occupant_flag = false;
	}
	
	bool Object;
	bool IR;
	
	current.Object_size = Seat_Distance - current.Distance;
	
	if (!Occupant_flag){
		switch (State) //Need to add in CO states
		{
			case Running: //Under 80 deg F`
			case Stopped:
			case TH0:
			if ((current.Object_size > Occupant_thickness) || current.Seat_load){
				Object = true;
			}
			if ((current.PIR)||(Thermal_parsed.detected)){
				IR = true;
			}
			if (Object && IR){
				return 1;
				Occupant_flag = true;
				//since there is a new occupant detected, save the states of the occupancy detection sensors
				previous = current;
				} else {
				return 0;
			}
			break;
			
			case TH1: //Above 80 deg F
			case TH2:
			case TH3:
			case TH4:
			if ((current.Object_size > Occupant_thickness) && current.Seat_load){
				Object = 1;
			}
			if (current.PIR || Thermal_parsed.detected){
				IR = 1;
			}
			if (Object || IR){
				return 1;
				Occupant_flag = true;
				} else {
				return 0;
			}
			break;
			
			default:
			break;
		}
	} else if (Occupant_flag){ //if an occupant was detected before, we check if the occupant has left
		switch (State) //Need to add in CO states
		{
			case Running: //Under 80 deg F`
			case Stopped:
			case TH0:
			//PIR will likely turn up negative on a second read if the occupant has kept still, so we do not check if the occupant has left using PIR. PIR only used to transition from no occupant to occupant.
			//if occupant no longer appears on ping sensor, update memory
			if(previous.Object_size > Occupant_thickness){
				if(current.Object_size < Occupant_thickness){
					Object = false; //since the flag can be set by either object sensor, the change in state of either object sensor can unset the flag
					previous.Object_size = current.Object_size;
				}
			}
			//if occupant no longer appears on seat load, update memory
			if(previous.Seat_load){
				if(!current.Seat_load){
					Object = false;
					previous.Seat_load = current.Seat_load;
				}
			}
			if(previous.IR){
				if(!current.IR){
					IR = false;
					previous.IR = current.IR;
				}
			}
			if (!Object && !IR){
				Occupant_flag = false;
				previous = current;
			}
			
			break;
			
			case TH1: //Above 80 deg F, using only object sensors to show that occupant has moved
			case TH2:
			case TH3:
			case TH4:
			if(previous.Object_size > Occupant_thickness){
				if(current.Object_size < Occupant_thickness){
					Object = false;
					previous.Object_size = current.Object_size;
				}
			}
			//if occupant no longer appears on seat load, update memory
			if(previous.Seat_load){
				if(!current.Seat_load){
					Object = false;
					previous.Seat_load = current.Seat_load;
				}
			}
			if (!Object){
				Occupant_flag = false;
				previous = current;
			}
			
			break;
			
			default:
			break;
		}
	}
}

void setup() {
	// put your setup code here, to run once:

	//Serial.begin(9600);
	//Serial.println(F("AMG88xx thermal camera!"));

	pinMode(PIR_read_pin, INPUT);
	pinMode(PIR_enable_pin, OUTPUT);
	pinMode(Ping_enable_pin, OUTPUT);
	pinMode(Alarm_Lights_pin, OUTPUT);
	pinMode(Car_unlock_pin, OUTPUT);
	pinMode(Car_door_open_pin, OUTPUT);
	pinMode(GPS_enable_pin, OUTPUT);
	
	digitalWrite(Ping_enable_pin, LOW);
	digitalWrite(PIR_enable_pin, LOW);
	digitalWrite(Alarm_Lights_pin, LOW);
	digitalWrite(Car_unlock_pin, LOW);
	digitalWrite(Car_door_open_pin, LOW);
	digitalWrite(GPS_enable_pin, LOW);
	
	Serial.begin(9600);
	Serial.println(F("Hello World!"));

	Thermal_setup();

}

//int Print_delay = 0;

float Felt_temp;


unsigned long Time_stop_start; // time variables are reserved exclusively for reading the onboard clock. This clock overflows approximately ever 50 days.

int Ping_calibration(){
	int Sample_size = 10;
	int Reading;
	
	for (int i = 0; i < Sample_size ; i++)
	{
		Reading += hcsr04.distanceInMillimeters();
	}
	
	Reading = Reading/Sample_size;
	
	return Reading;
	
};

void loop() { //main loop here
	
	//guardian statement for running car
	if (State != Running){
		
		//check temperature
		Felt_temp = Get_Felt_Temperature();
		
		if (Testing_mode){
			Serial.println("input felt temperature");
			while(!Serial.available());
			String Testing_input = Serial.readString();
			Serial.println(Testing_input);
			Felt_temp = Testing_input.toInt();
		}
		
		if (!Testing_mode){
			MPU_read = Get_MPU_Data();
			} else {
			Serial.println("Car not running accel check");
			Serial.println("Input MPU force; 1 or 2g");
			while(!Serial.available()); //block program while waiting for serial input
			int Testing_input = Serial.read();
			Serial.println(Testing_input);
			MPU_read.gForceX = Testing_input - 48;
		}
		if ((MPU_read.gForceX + MPU_read.gForceY + MPU_read.gForceZ) >= 1.1){ //car is running if g forces read from all three axis is above 1.1G
			State = Running;
		}
	}
	if (User_dismiss){ //user dismissal flag, resets the next time the car is said to be in use
		State = TH0;
	}
	

	switch (State)
	{
		case Running:
		
		//reset user dismissal flag when car is in use
		User_dismiss = false;
		
		//guardian statement: car stopped
		if (!Testing_mode){
			MPU_read = Get_MPU_Data();
			} else {
			Serial.println("Car Running");
			Serial.println("Input MPU force; 1 or 2g");
			while(!Serial.available()); //block program while waiting for serial input
			int Testing_input = Serial.read();
			Serial.println(Testing_input);
			MPU_read.gForceX = Testing_input - 48;
		}
		
		if ((MPU_read.gForceX + MPU_read.gForceY + MPU_read.gForceZ) <= 1.1){ //if total g forces read is less than 1.1 g, the car is stopped. Most often should come out to 1g, the force of gravity
			State = Stopped;
			Time_stop_start = millis(); //On transition to stop, record start timer so we only start systems if the accelerometer records 1G for more than 10 seconds.
		}
		
		break; //this does not account for traffic lights and stop signs, need to figure out how to incorporate WiFi to check for the driver
		
		case Stopped:
		
		if (Testing_mode){
			Serial.println("Car Stopped");
			//Time_stop_start = 0;
		}
		
		if (abs(millis() - Time_stop_start) > 10000){ //if Accelerometer has been stopped for 10 seconds
			
			//turn on GPS to get fix on location
			if (Occupant_flag){
				digitalWrite(GPS_enable_pin, HIGH);
				
				//check for sun
			}
			
			if (Felt_temp > 80){
				State = TH1;
				} else {
				State = TH0;
			}
		}
		
		break;
		
		case TH0: //no threat to life
		
		if (Testing_mode){
			Serial.println("TH0");
		}
		//check occupancy
		if (Occupant_detect()){
			Occupant_flag = true;
			} else if (!Occupant_detect()){
			/*Calibrate Ping sensor*/
			
			if(!Testing_mode)
			Seat_Distance = Ping_calibration(); //the call to Ping_calibration when the sensor is not connected will crash the system because of its blocking nature
		}
		
		if (Felt_temp > 80){
			State = TH1;
		}
		
		
		break;
		
		case TH1:
		
		if (Testing_mode){
			Serial.println("TH1");
		}
		
		//send notification
		
		if (Felt_temp > 86){
			State = TH2;
			} else if (Felt_temp < 80){
			State = Stopped;
			Deescalation_dismiss = 1;
		}
		break;
		
		case TH2:
		
		if (Testing_mode){
			Serial.println("TH2");
		}
		
		//lower windows, send notification to owners
		
		if (Felt_temp > 91){
			State = TH3;
		} else if (Felt_temp < 86)
		{
			State = Stopped;
			Deescalation_dismiss = 1;
		}
		break;
		
		case TH3:
		
		if (Testing_mode){
			Serial.println("TH3");
		}
		
		//911 alert, lights and alarms, send notification to owners
		digitalWrite(Car_unlock_pin, HIGH);
		digitalWrite(Alarm_Lights_pin, HIGH);
		
		if (Felt_temp > 105){
			State = TH4;
		} else if (Felt_temp < 91)
		{
			State = Stopped;
			Deescalation_dismiss = 1;
		}
		break;
		
		case TH4:
		
		if (Testing_mode){
			Serial.println("TH4");
		}
		
		//open car door
		
		if (Felt_temp < 105){
			State = Stopped;
			Deescalation_dismiss = 1;
		}
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