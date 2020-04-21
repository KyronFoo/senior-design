/*
 * Occupant.c
 *
 * Created: 4/21/2020 1:28:16 PM
 *  Author: alok
 */ 

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

