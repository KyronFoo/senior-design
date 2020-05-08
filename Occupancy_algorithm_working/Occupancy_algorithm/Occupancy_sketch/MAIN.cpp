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

int Distance;
int Seat_Distance = 1000; //to be changed per car based on calibration
int PIR;
int Seat_load;
int IR;
int Detection_flag = false;
int Sleep_time = 290000; //4:50 minutes of sleep time
bool User_dismiss = 0; //user override flag
bool Deescalation_dismiss = 0; //software action override flag when temperatures are lowering as opposed to rising
bool Testing_mode = 0; //set this to 1 to enable testing mode, where all data is inputted and outputted via the serial port. 

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


bool Occupant_detect(){
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
bool Occupant_flag = false;

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
		MPU_read = Get_MPU_Data();
		
		if ((MPU_read.gForceX + MPU_read.gForceY + MPU_read.gForceZ) <= 1.1){ //if total g forces read is less than 1.1 g, the car is stopped. Most often should come out to 1g, the force of gravity
			State = Stopped;
			Time_stop_start = millis(); //On transition to stop, record start timer so we only start systems if the accelerometer records 1G for more than 10 seconds.
		}
		break; //this does not account for traffic lights and stop signs, need to figure out how to incorporate WiFi to check for the driver
		
		case Stopped:
		
		if (abs(millis() - Time_stop_start) > 10000){ //if Accelerometer has been stopped for 10 seconds
			
			//turn on GPS to get fix on location
			digitalWrite(GPS_enable_pin, HIGH);
			
			//check for sun
			
			//check temperature
			Felt_temp = Get_Felt_Temperature();
			
			if (Felt_temp > 80){
				State = TH1;
				} else {
				State = TH0;
			}
			break;
			
			case TH0: //no threat to life
			//check occupancy
			if (Occupant_detect()){
				Occupant_flag = true;
				} else if (!Occupant_detect()){
				/*Calibrate Ping sensor*/
				Seat_Distance = Ping_calibration();
			}
			
			if (Felt_temp > 80){
				State = TH1;
			}
		}
		
		break;
		
		case TH1:
		
		//send notification
		
		if (Felt_temp > 86){
			State = TH2;
			} else if (Felt_temp < 80){
			State = Stopped;
			Deescalation_dismiss = 1;
		}
		break;
		
		case TH2:
		
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