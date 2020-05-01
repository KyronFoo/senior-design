/*
 * GPS.h
 *
 * Created: 5/1/2020 1:37:46 AM
 *  Author: syngr
 */ 


#ifndef GPS_H_
#define GPS_H_

#define GPSSerial Serial2
#define GPSECHO false

struct GPS_data{
	bool fix;
	int quality;
	int latitude;
	char N_S; //north or south specifier for latitude
	int longitude;
	char E_W; //east west specifier for longitude
	int satellites;
	bool error;
	};

GPS_data GPS_read(); 

#endif /* GPS_H_ */