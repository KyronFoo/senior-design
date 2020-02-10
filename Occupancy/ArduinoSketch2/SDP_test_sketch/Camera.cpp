/*
 * Camera.cpp
 *
 * Created: 2/9/2020 10:10:44 PM
 *  Author: syngr
 */ 
#include "Camera.h"
#include "SoftwareSerial.h"
#include <Arduino.h>

void Camera_setup(Adafruit_AMG88xx amg){
	bool status;
	
	digitalWrite(Camera_enable_pin, HIGH);
	delay(100);
	
	// default settings
	status = amg.begin();
	if (!status) {
		Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
		while (1);
	}
	
	Serial.println("-- Pixels Test --");

	Serial.println();

	delay(100); // let sensor boot up
}

void Camera_read(Adafruit_AMG88xx amg, float *pixels){
	//read all the pixels
	amg.readPixels(pixels);

	Serial.print("[");
	for(int i=1; i<=AMG88xx_PIXEL_ARRAY_SIZE; i++){
		Serial.print(pixels[i-1]);
		Serial.print(", ");
		if( i%8 == 0 ) Serial.println();
	}
	Serial.println("]");
	Serial.println();
	
	digitalWrite(Camera_enable_pin, LOW);

	//delay a second
	delay(1000);
}

