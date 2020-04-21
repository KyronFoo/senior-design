#include <Arduino.h>
#include "Sketch.h"

HCSR04 hcsr04(TRIG_PIN, ECHO_PIN, 20, 4000); //setup ping sensor
struct Thermal_SizeTemp_Struct Thermal_parsed;

int Distance;
int Seat_Distance = 1000; //to be changed per car based on calibration
int PIR;
int Seat_load;
int IR;
int Detection_flag = false;
int Sleep_time = 290000; //4:50 minutes of sleep time

states State = Stopped;

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

	switch (State)
	{
		case Running:
		//accel read is 0
		//driver read is 0		
		break;
		
		case Stopped:
		
		/*check for sun*/
		
		break;
		
		case TH0:
		if (!Occupant_detect(State)){
			/*Calibrate Ping sensor*/
			Seat_Distance = Ping_calibration(); 	
		}
				
		break;
		
		default:
		State = Running;
		break;
	}

	/*
	
	/*this was for testing*/
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
	/*end testing code*/
	
}

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

int Occupant_detect(states State){
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

