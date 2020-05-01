/*
 * Temp_Humidity.c
 *
 * Created: 4/30/2020 10:30:24 PM
 *  Author: syngr
 */ 

#include "Temp_Humidity.h"
#include "SparkFun_HIH4030.h"
#include "TMP36.h"

TMP36 myTMP36(A1, 5.0);

HIH4030 sensorSpecs(Humidity_read_pin, HIH4030_SUPPLY);


float Get_temperature(){
	float temp;
	temp = myTMP36.getTempF();
	return temp;
}

float Get_humidity(){
	float humidity;
	humidity = sensorSpecs.getSensorRH();
	return humidity; 
}

float Get_Felt_Temperature(){
	float FT;
	FT = Get_temperature();
	return FT;
}
