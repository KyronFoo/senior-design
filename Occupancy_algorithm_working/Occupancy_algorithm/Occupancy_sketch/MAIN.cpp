﻿/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

//#include <Adafruit_GPS.h>
#include <hcsr04.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <Adafruit_AMG88xx.h>
#include "IR_cam.h"
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include "Accelerometer.h"
#include "Temp_Humidity.h"
#include <string.h>
#include <Stream.h>
#include <stdio.h>
#include <stdlib.h>
#include "Fona.h"
#include "GPS.h"

#define Power_pin 36//connects to power pin on telecom module
#define TRIG_PIN 27
#define ECHO_PIN 29
#define PIR_read_pin 31
#define CO_read_pin A3
//#define Telecom_enable_pin 49
#define Ping_enable_pin 49
#define PIR_enable_pin 53
#define Camera_enable_pin 35
#define Alarm_Lights_Unlock_pin 50
#define GPS_enable_pin 39
#define WiFi_wake_pin 30
#define Fona_power_status_pin 29
#define MPU_enable_pin 33
#define Fona_serial Serial1
#define GPS_serial Serial2
#define Wifi_serial Serial3

#define MPU_threshold 1.2

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
bool Calibrate_enable = 0;
bool TH1_enable = 1;
bool TH2_enable = 1;
bool TH3_enable, TH4_enable = 1;

bool Testing_mode = false; //set this to 1 to enable testing mode, where all data is inputted and outputted via the serial port.
bool Print_mode = true; //set this to 1 to enable printout mode, where all data from hardware and decisions are printed out.

enum states {
	Running, //accelerometer detects movement
	Pause, //accelerometer detects no movement
	Stopped, //accelerometer detects no movement and no driver detected, determine if state machine needed or go to sleep
	CO1, // 70 ppm
	CO2, // 150 ppm
	TH0, // No threat
	TH1, // 80 degrees, send notification
	TH2, // 86 degrees, lower windows
	TH3, // 91 degrees, call 911
	TH4, // 105 degrees, Open door
};

enum Temp_zone {
	Cool,
	Hot,
};

enum Wifi_command{
	Driver_check,
	Open_door,
	Seat_check
};

states State = Running;
Temp_zone Temperature_zone = Cool;
Wifi_command Wee_command = Driver_check;

bool Occupant_flag = false; //This flag is used both in occupant detect and in the main loop
int Driver_flag = 3; //This flag is set by incoming data from the wifi module. Since we begin in the running state, this flag starts in the positve
bool PIR_flag = false; //the PIR sensor sets this flag once a minute. Is used to detect positives above 80F
bool PIR_enable  = false; //determines if PIR is in use logically
bool PIR_on = false; //used by the sleep loop to bounce between pir on and off
occupancy_data current;
occupancy_data previous;
GPS_data gps;

bool Wifi_comms(Wifi_command command);

bool Occupant_detect(){
	
	if (Print_mode)
	{
		Serial.println("occupant detect");
	}
	
	if (!Testing_mode){
		//turn on Ping and IR camera
		digitalWrite(Ping_enable_pin, HIGH);
		digitalWrite(Camera_enable_pin, HIGH);
		delay(1000); //allow camera and Ping to boot
		Thermal_setup(); //blocking code, communicate with IR camera
		//digitalWrite(PIR_enable_pin, LOW);
		
		//sun check
		current.Distance = hcsr04.distanceInMillimeters();
		if (Print_mode){
			Serial.print("distance read: ");
			Serial.println(current.Distance);
		}
		current.PIR = PIR_flag; //Tie PIR to ISR and turn on using the sleep timer.
		current.Seat_load = 1; // hard coded for positive reading, to be backed by logic when module is designed
		Thermal_parsed = Thermal_read();
		current.IR = Thermal_parsed.detected;
		
		if (Print_mode){
			Serial.println("Occupancy statuses: ");
			Serial.print("Distance read: "); Serial.println(current.Distance);
			Serial.print("PIR read: "); Serial.println(current.PIR);
			Serial.print("Seat load: "); Serial.println(current.Seat_load);
			Serial.print("Thermal camera: "); Serial.println(Thermal_parsed.detected);
			delay(1000);
		}
		
	}
	
	bool Object;
	bool IR = false;
	
	current.Object_size = Seat_Distance - current.Distance;
	
	if (!Occupant_flag){
		switch (Temperature_zone) //Need to add in CO states
		{
			case Cool: //under 80 degrees
			
			if (Print_mode){
				Serial.println("Occupancy detection < 80");
				delay (500);
			}
			
			if ((current.Object_size > Occupant_thickness) | current.Seat_load){
				Object = true;
			}
			if (current.PIR | Thermal_parsed.detected){
				IR = true;
				
				if (Print_mode)
				{
					Serial.print("IR flag: "); Serial.println(IR);
					delay(1000);
				}
				
			}
			if (Testing_mode){
				//Serial.println("Enter occupant flag 1 or 0");
				//while(!Serial.available());
				Object = Serial.read() - 48;
				Serial.println("Enter IR flag");
				while(!Serial.available());
				IR = Serial.read() - 48;
			}
			if (Object & IR){
				
				Occupant_flag = true;
				if (Print_mode)
				{
					Serial.print("occupant flag: "); Serial.println(Occupant_flag);
				}
				//since there is a new occupant detected, save the states of the occupancy detection sensors
				previous = current;
				//State = TH0; //there is no need to reset under 80 degrees
				TH2_enable, TH1_enable, TH3_enable, TH4_enable = true;
				return 1;
				} else {
				return 0;
			}
			break;
			
			case Hot: //above 80 degrees
			
			if (Print_mode){
				Serial.println("Occupancy detection > 80");
				delay (500);
			}
			
			if ((current.Object_size > Occupant_thickness) & current.Seat_load){
				Object = 1;
			}
			if (current.PIR | Thermal_parsed.detected){
				IR = 1;
			}
			if (Testing_mode){
				Serial.println("Enter object flag 1 or 0");
				while(!Serial.available());
				Object = Serial.read() - 48;
				Serial.println("Enter IR flag");
				while(!Serial.available());
				IR = Serial.read() - 48;
			}
			if (Object || IR){
				switch (State){
					case TH1:
					case TH2:
					case TH3:
					case TH4:
					State = TH0;
					break;
				}
				
				Occupant_flag = true;
				return 1;
				} else {
				return 0;
			}
			break;
			
			default:
			break;
		}
		} else if (Occupant_flag){ //if an occupant was detected before, we check if the occupant has left
		switch (Temperature_zone) //Need to add in CO states
		{
			case Cool: //under 80
			
			if (Print_mode){
				Serial.println("Occupancy cancel <80");
				delay(500);
			}
			
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
			if (Testing_mode){
				Serial.println("Enter object flag 1 or 0");
				while(!Serial.available());
				Object = Serial.read() - 48;
				Serial.println("Enter IR flag");
				while(!Serial.available());
				IR = Serial.read() - 48;
			}
			if (!Object & !IR){
				Occupant_flag = false;
				previous = current;
			}
			
			break;
			
			case Hot: //above 80
			
			if (Print_mode){
				Serial.println("Occupancy cancel > 80");
				delay(500);
			}
			
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
			if (Testing_mode){
				Serial.println("Enter occupant flag 1 or 0");
				while(!Serial.available());
				Object = Serial.read() - 48;
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
	 set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	 cli();
	 
	 TCCR1B = (1<<CS10) | (1<<CS12);
	 
	pinMode(LED_BUILTIN, OUTPUT);	
	pinMode(PIR_read_pin, INPUT);
	pinMode(PIR_enable_pin, OUTPUT);
	pinMode(Ping_enable_pin, OUTPUT);
	pinMode(Alarm_Lights_Unlock_pin, OUTPUT);
	pinMode(WiFi_wake_pin, OUTPUT);
	pinMode(GPS_enable_pin, OUTPUT);
	pinMode(Camera_enable_pin, OUTPUT);
	
	digitalWrite(LED_BUILTIN, LOW);
	digitalWrite(Ping_enable_pin, LOW);
	digitalWrite(PIR_enable_pin, HIGH); //PIR pin is high for testing. set low for implementation
	digitalWrite(Alarm_Lights_Unlock_pin, LOW);
	digitalWrite(WiFi_wake_pin, HIGH);
	digitalWrite(GPS_enable_pin, HIGH); //pin is high for testing, set low for implementation
	digitalWrite(Camera_enable_pin, LOW);
	
	Serial.begin(9600);
	if (Print_mode)
	{
		Serial.println(F("Setting up"));
	}
	
	Fona_serial.begin(9600);
	GPS_serial.begin(9600);
	Wifi_serial.begin(9600);

	//Thermal_setup();
	setupMPU();
	if (Print_mode){
		Serial.println("MPU in");
	}
	//Fona_setup();
}

//int Print_delay = 0;

float Felt_temp;
int C_O;


unsigned long Time_stop_start; // time variables are reserved exclusively for reading the onboard clock. This clock overflows approximately ever 50 days.

int Ping_calibration(){
	
	if (Print_mode)
	{
		Serial.println("Ping calibration");
	}
	
	int Sample_size = 10;
	int Reading;
	
	digitalWrite(Ping_enable_pin, HIGH);
	delay(500); //allow sensor to stabilize
	
	for (int i = 0; i < Sample_size ; i++)
	{
		Reading += hcsr04.distanceInMillimeters();
	}
	
	Reading = Reading/Sample_size;
	
	digitalWrite(Ping_enable_pin, LOW); //turn off sensor after calibration
	
	return Reading;
	
};

void loop() { //main loop here
	
	bool Go_to_sleep = false; //start with this false, to be set true by state dependent logic. This variable is declared inside the loop because it does not have be consistent between loops.
	
	if (Print_mode)
	{
		Serial.println("wake");
	}
	
	//check wifi, 3G, and GPS
	if (Fona_serial.available()){ //Implement flow control
		//insert function here to read and update main device with 3G data
		//String Read_3G = Serial1.readString();
		Fona_SMS_recieve();
		
		if (Print_mode){
			Serial.println("receiving SMS");
		}
		
	}
	
	if (GPS_serial.available()){
		
		if (Print_mode){
			Serial.println("receiving GPS");
		}
		
		gps = GPS_read();
		if (Print_mode){
			
			Serial.print("Fix: "); Serial.print((int)gps.fix);
			Serial.print(" quality: "); Serial.println((int)gps.quality);
			if (gps.fix) {
				Serial.print("Location: ");
				Serial.print(gps.latitude, 4); Serial.print(gps.N_S);
				Serial.print(", ");
				Serial.print(gps.longitude, 4); Serial.println(gps.E_W);
				Serial.print("Satellites: "); Serial.println((int)gps.satellites);
			}
		}
	}
	
	
	//guardian statement for not running car
	if (State != Running){
		
		//since we are not running, check if the GPS needs to be on. GPS only needs to be on if occupant present
		if (!Occupant_flag){
			if (digitalRead(GPS_enable_pin)){
				digitalWrite(GPS_enable_pin, LOW);
				PIR_enable = false;
			}
		}
		//check temperature
		Felt_temp = Get_Felt_Temperature();
		if (Thermal_parsed.sunlight){
			Felt_temp = Felt_temp + 15;
		}
		
		//check CO level
		
		if(Print_mode){
			Serial.print("temperature "); Serial.println((int) Felt_temp);
		}
		
		if (Testing_mode){ //receive serial inputs in testing mode
			Serial.println("input felt temperature");
			while(!Serial.available());
			String Testing_input = Serial.readString();
			Serial.println(Testing_input);
			Felt_temp = Testing_input.toInt();
		}
		
		if (Felt_temp < 80){
			Temperature_zone = Cool;
		} else if (Felt_temp >= 80)
		{
			Temperature_zone = Hot;
		}
		
		//In real mode, get MPU data
		if (!Testing_mode){
			MPU_read = Get_MPU_Data();
			if (Print_mode){
				Serial.print("MPU: "); Serial.print(MPU_read.gForceX); Serial.print(MPU_read.gForceY); Serial.println(MPU_read.gForceZ);
			}
			} else {
			Serial.println("Car not running accel check");
			Serial.println("Input MPU force; 1 or 2g");
			while(!Serial.available()); //block program while waiting for serial input
			int Testing_input = Serial.read();
			//Serial.println(Testing_input);
			MPU_read.gForceX = Testing_input - 48;
		}
		if ((MPU_read.gForceX + MPU_read.gForceY + MPU_read.gForceZ) >= MPU_threshold){ //car is running if g forces read from all three axis is above 1.1G
			State = Running;
			
			if (Print_mode)
			{
				Serial.print("Movement detected");
			}
			
		}
	}
	if (User_dismiss){ //user dismissal flag, resets the next time the car is said to be in use
		State = TH0;
	}
	
	//Run Occupancy sensors each wake up
	Occupant_detect();

	switch (State)
	{
		case Running:
		
		if (Print_mode){
			Serial.println("State: Running");
			delay(500);
		}
		
		if (!TH1_enable){ //all actions will result in TH1 having activated
			TH1_enable, TH2_enable, TH3_enable, TH4_enable = true;
		}
		
		//reset user dismissal flag when car is in use
		User_dismiss = false;
		
		//turn off GPS, WiFi, and PIR
		if (digitalRead(GPS_enable_pin)){
			digitalWrite(GPS_enable_pin, LOW);
		}
		if (digitalRead(PIR_enable_pin)){
			digitalWrite(PIR_enable_pin, LOW);
		}
		if (!digitalRead(WiFi_wake_pin)){
			digitalWrite(WiFi_wake_pin, HIGH);
		}
		
		//guardian statement: car stopped
		if (!Testing_mode){
			MPU_read = Get_MPU_Data();
			if (Print_mode){
				Serial.print("MPU"); Serial.println((MPU_read.gForceZ + MPU_read.gForceY + MPU_read.gForceX));
			}
			//Occupant_flag = Occupant_detect();
			} else {
			Serial.println("Car Running");
			Serial.println("Input MPU force; 1 or 2g");
			while(!Serial.available()); //block program while waiting for serial input
			int Testing_input = Serial.read();
			Serial.println(Testing_input);
			MPU_read.gForceX = Testing_input - 48;
		}
		
		if ((MPU_read.gForceX + MPU_read.gForceY + MPU_read.gForceZ) <= MPU_threshold){ //if total g forces read is less than 1.1 g, the car is stopped. Testing reveals accel reads 1.06 G at rest.
			State = Pause;
			Driver_flag = 3; //reset driver flag before driver presence check
			Time_stop_start = millis(); //On transition to stop, record start timer so we only start systems if the accelerometer records 1G for more than 10 seconds.
			if (Print_mode){
				Serial.println("No movement and no driver");
				delay(500);
			}
			} else { //if the MPU reads 1G, we transition to the next state, otherwise we go to sleep
			Go_to_sleep = true;
		}
		
		break; //this does not account for traffic lights and stop signs, need to figure out how to incorporate WiFi to check for the driver
		
		case Pause: //This state does not go to sleep because it needs to wait 10 seconds for the MPU
		
		if (Print_mode){
			Serial.println("State: Pause");
			//Serial.println("time:"); Serial.println(Time_stop_start);
			delay(500);
		}
		
		if ((millis() - Time_stop_start) > 10000){ //if Accelerometer has been stopped for 10 seconds
			
			if (Print_mode){
				Serial.println("timeout accelerometer");
			}
			
			//turn on GPS to get fix on location and run occupant check
			if(Occupant_detect())
			{
				digitalWrite(GPS_enable_pin, HIGH);
			}
			
			//turn on GPS
			digitalWrite(WiFi_wake_pin, LOW);
			Wee_command = Driver_check;
			while(!Wifi_comms(Wee_command)); //wait until wifi is finished checking
			
			Serial3.write('D');
			
			
			if (Print_mode){
				Serial.print(Serial3.read());
				Serial.print("Driver flag: "); Serial.println(Driver_flag);
				delay(1000);
			}
			
			if (Testing_mode){
				Serial.println("input driver flag");
				while(!Serial.available());
				Driver_flag = Serial.read() - 48;
			}
			
			if(!Driver_flag){
				State = Stopped; // if there is no driver found, turn on the wifi module again to check seat loaders
				
			} else if (Driver_flag == true) //if a driver is found, go to sleep
			{
				Go_to_sleep = true;
				State = Pause;
				
			} //if nothing has been received from the wifi module, run through the loop again
		}
		
		break;
		
		case Stopped: //this state does not go to sleep either. simply goes through the transition to th0 or th1
		
		digitalWrite(WiFi_wake_pin, LOW); //wake wifi to check for seat load
		Wee_command = Seat_check;
		while (!Wifi_comms(Wee_command)); //wait until seat check is finished
		
		if (Print_mode){
			Serial.println("State: Stopped");
			delay(500);
		}
		
		if (Testing_mode){
			Serial.println("Car Stopped");
			//Time_stop_start = 0;
		}
		
		if (Felt_temp >= 80){
			State = TH1;
			} else {
			State = TH0;
		}
		
		Calibrate_enable = true; //enable calibration in TH0
		Deescalation_dismiss = false;
		
		break;
		
		case TH0: //no threat to life
		
		if (Print_mode){
			Serial.println("State: TH0");
			delay(500);
		}
		
		//turn off PIR in TH0 to save power
		if (digitalRead(PIR_enable_pin)){
			digitalWrite(PIR_enable_pin, LOW);
			PIR_enable = false;
		}
		
		if (Deescalation_dismiss)
		{
			Deescalation_dismiss = false;
		}
		
		if (Testing_mode){
			Serial.println("TH0");
		}
		
		if (!Occupant_flag && Calibrate_enable){
			/*Calibrate Ping sensor*/
			Calibrate_enable = false; //run calibration only once
			
			if(!Testing_mode)
			Seat_Distance = Ping_calibration(); //the call to Ping_calibration when the sensor is not connected will crash the system because of its blocking nature
			
			} else if (Occupant_flag){ //if occupant detected, check that GPS and 3G modules are turned on.
			
			if (!digitalRead(GPS_enable_pin))
			{
				digitalWrite(GPS_enable_pin, HIGH);
			}
			if (!digitalRead(Fona_power_status_pin))
			{
				//Fona connection is not ready yet
				//Fona_setup(); //turns on and sets up 3G. Is blocking for 7 seconds. We do not receive GPS during this time. PIR should be attached to an interrupt to detect motion
			}
			
		}
		
		if (Felt_temp >= 80 && !User_dismiss){
			State = TH1;
			Deescalation_dismiss = false;
			//turn on wifi module for check of seat loaders
			digitalWrite(WiFi_wake_pin, LOW);
			Wee_command = Seat_check;
			while(!Wifi_comms(Wee_command));
			} else { //if there is no transition, go to sleep
			Go_to_sleep = true;
		}
		
		
		break;
		
		case TH1:
		
		if (Print_mode){
			Serial.println("State: TH1");
			delay(500);
		}
		
		//turn on PIR from this threat level forward
		PIR_enable = true;
		
		
		if (Testing_mode){
			Serial.println("TH1");
		}
		
		//send notification
		if (Occupant_flag && TH1_enable){
			Fona_Send_sms((int) Felt_temp, (int) Get_humidity(), C_O, Occupant_flag, gps.latitude, gps.N_S, gps.longitude, gps.E_W);
			TH1_enable = false;
			
			if (Print_mode){
				Serial.println("Sending SMS");
				delay(500);
			}
		}
		
		if (Felt_temp > 86){
			State = TH2;
			Deescalation_dismiss = false;
			digitalWrite(WiFi_wake_pin, LOW);
			Wee_command = Seat_check;
			while(!Wifi_comms(Wee_command));
			} else if (Felt_temp < 80){
			State = Stopped;
			Deescalation_dismiss = true;
			} else { //if there is no transition, go to sleep
			Go_to_sleep = true;
		}
		break;
		
		case TH2:
		
		if (Print_mode){
			Serial.println("State: TH2");
			delay(500);
		}
		
		if (Testing_mode){
			Serial.println("TH2");
		}
		
		//lower windows, send notification to owners
		//lower windows never implemented
		if (Print_mode){
			Serial.println("Lower Windows");
			delay(500);
		}
		
		if (Felt_temp > 91){
			State = TH3;
			Deescalation_dismiss = false;
			digitalWrite(WiFi_wake_pin, LOW);
			Wee_command = Seat_check;
			while(!Wifi_comms(Wee_command));
		} else if (Felt_temp < 86)
		{
			State = Stopped;
			Deescalation_dismiss = true;
			}else { //if there is no transition, go to sleep
			Go_to_sleep = true;
		}
		break;
		
		case TH3:
		
		if (Print_mode){
			Serial.println("State: TH3");
			delay(500);
		}
		
		if (Testing_mode){
			Serial.println("TH3");
		}
		
		//911 alert, lights and alarms, send notification to owners
		if(TH3_enable && Occupant_flag){
			
			if (Print_mode){
				Serial.println("911 alert");
				delay(500);
			}
			
			if (!digitalRead(Alarm_Lights_Unlock_pin))
			{
				//press pin for 1 second
				digitalWrite(Alarm_Lights_Unlock_pin, HIGH);
				delay (1000);
				digitalWrite(Alarm_Lights_Unlock_pin, LOW);
			}
			
			Fona_Send_sms((int) Felt_temp, (int) Get_humidity(), C_O, Occupant_flag, gps.latitude, gps.N_S, gps.longitude, gps.E_W);
			
			TH3_enable = false;
		}
		
		if (Felt_temp > 105){
			State = TH4;
			Deescalation_dismiss = false;
			digitalWrite(WiFi_wake_pin, LOW);
			Wee_command = Seat_check;
			while(!Wifi_comms(Wee_command));
		} else if (Felt_temp < 91)
		{
			State = Stopped;
			Deescalation_dismiss = 1;
			}else { //if there is no transition, go to sleep
			Go_to_sleep = true;
		}
		break;
		
		case TH4:
		
		if (Print_mode){
			Serial.println("State: TH4");
			delay(500);
		}
		
		if (Testing_mode){
			Serial.println("TH4");
		}
		
		//open car door
		if (TH4_enable && Occupant_flag)
		{
			Serial3.write("O");
			
			if (Print_mode){
				Serial.println("Open Door");
				delay(500);
			}
			
		}
		
		if (Felt_temp < 105){
			State = Stopped;
			Deescalation_dismiss = 1;
		}
		else { //if there is no transition, go to sleep
			Go_to_sleep = true;
		}
		break;
		
		default:
		State = Running;
		Go_to_sleep = true;
		break;
	}
	

	/*
	sleep here for 1 minute
	*/
	
	
	
	if (Go_to_sleep && digitalRead(WiFi_wake_pin)) //check that the sleep enable flag is high and that the Wifi module is off and not transmitting.
	//in the future, wifi receive should be tied to an interrupt while the mega is asleep so that the sleep cycle is not disrupted - kf 6/2/20
	{
		
		if(Testing_mode || Print_mode){
			Serial.println("Sleep");		
		delay(1000); //delay 10 seconds for testing
		} else {			
				 sleep_enable();
				 sei();
				 sleep_cpu();
				 sleep_disable();
			 			 
		sei();
		}
	}
	
	//char text[140];
	////char state[32];
	//
	//int Humidity = 50;
	//sprintf(text, "FT = %d, H = %d, CO = %d, OC = %d, lat = %f, NS = %c, lon = %f, EW = %c", Felt_temp, Humidity, C_O, Occupant_flag, gps.latitude, gps.N_S, gps.longitude, gps.E_W);
	//
	//Serial.println(text);
	
	//if (PIR_on && PIR_enable)
	//{
	//digitalWrite(Ping_enable_pin, HIGH);
	//PIR_on = false;
	//} else if (!PIR_on) {
	//PIR_on = true;
	//}
	//

}

int sleep=0;

ISR(TIMER1_OVF_vect){
	//wake up Arduino when Timer 1 overflows, with 1024 pre-scalar, overflows every 4 seconds
	if (sleep < 15){ //if a minute isn't up yet, go back to sleep
		sleep_cpu();
	} else {
		sleep = 0; //reset the sleep counter and go through the rest of the program
	}
}





bool Wifi_comms(Wifi_command command){
	if (Wifi_serial.available()){
		
		String S = Wifi_serial.readString();
		
		switch(command){
			case Open_door: //open the drivers door
			if (S == "UPDATING2" || S == "OPTION2"){ //these indicate that the CDOM is ready to take the command to open the door
				Serial3.write('O'); //open door
				} else {
				Serial3.write('D'); //the CDOM is commanded to check for a driver first
			}
			break;
			
			case Driver_check:
			Serial3.write('D'); //the CDOM is commanded to check for a driver
			break;
			
			case Seat_check:
			Serial3.write('S');
			break;
		}
		
		char c = S[0]; //take the first character of the string
		if (Print_mode){
			Serial.print(c);
		}
		
		if (Print_mode){
			Serial.println("receiving WiFi char");
		}
		
		digitalWrite(WiFi_wake_pin, HIGH); //The AP SM puts the device to sleep after sending feedback, so write high to prepare to wake the wifi module later.
		
		
		switch (c){
			case 'Y': //driver present
			Driver_flag = true;
			if (command = Driver_check){
				return 1;
			}
			break;
			
			case 'N': //driver not present
			Driver_flag = false;
			if (command = Driver_check){
				return 1;
			}
			break;
			
			case 'S': //Seat Load positive
			current.Seat_load = true;
			return 1;
			break;
			
			case 's': //Seat Load negative
			current.Seat_load = false;
			return 1;
			break;
			
			case 'O': //driver door open successful
			return 1;
			break;
			
			case 'R': //order received
			return 1;
			break;
			
			case 'B': //baby
			return 1;
			break;
			
			case 'b': //no baby
			return 1;
			break;
			
			default:
			break;
			
		}
		
	}
	
	return 0;
	
}
