/*
 * GPS.cpp
 *
 * Created: 5/1/2020 1:47:41 AM
 *  Author: syngr
 */ 

#include "GPS.h"
#include <Adafruit_GPS.h>

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

//uint32_t timer; Since polling is determined by the main device, we don't need to depend on the timer to print information

void GPS_setup(){
	// 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
	GPS.begin(9600);
	// uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
	GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
	// uncomment this line to turn on only the "minimum recommended" data
	//GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
	// For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
	// the parser doesn't care about other sentences at this time
	// Set the update rate
	GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
	// For the parsing code to work nicely and have time to sort thru the data, and
	// print it out we don't suggest using anything higher than 1 Hz

	// Request updates on antenna status, comment out to keep quiet
	GPS.sendCommand(PGCMD_ANTENNA);

	delay(1000);

	// Ask for firmware version
	GPSSerial.println(PMTK_Q_RELEASE);
	
	//timer = millis();
}

GPS_data GPS_read(){
	char c = GPS.read();
	GPS_data data;
	data.error = false;
	
	if (GPS.newNMEAreceived()){
		if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
		data.error = true;
		return data;
	}	
	
	data.fix = GPS.fix; 
	data.quality = GPS.fixquality; 
	
	if(GPS.fix){
		data.latitude = GPS.latitude;
		data.N_S = GPS.lat;
		data.longitude = GPS.longitude;
		data.E_W = GPS.lon;
		data.satellites = GPS.satellites;
	}
	
	return data;
}