/*
 * Temp_Humidity.h
 *
 * Created: 4/30/2020 10:28:14 PM
 *  Author: syngr
 */ 


#ifndef TEMP_HUMIDITY_H_
#define TEMP_HUMIDITY_H_

#define Temp_read_pin  A0
#define Humidity_read_pin A1
#define HIH4030_SUPPLY 5

float Get_Felt_Temperature();
float Get_temperature();
float Get_humidity();

#endif /* TEMP_HUMIDITY_H_ */