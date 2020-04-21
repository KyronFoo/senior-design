/*
 * Sketch.h
 *
 * Created: 4/21/2020 1:07:17 PM
 *  Author: alok
 */ 


#ifndef SKETCH_H_
#define SKETCH_H_

#include <Adafruit_GPS.h>
#include <hcsr04.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include <Adafruit_AMG88xx.h>
#include "IR_cam.h"
#include <avr/sleep.h>
#include "Environmental.h"
//#include "Occupant.h"


//#define Telecom_TX  //connects to rx pin on telecom module // User hardware serial 1
//#define Telecom_RX  //connects to tx pin on telecom module
#define Power_pin 23//connects to power pin on telecom module
#define TRIG_PIN 29
#define ECHO_PIN 27
#define PIR_read_pin 31
//#define Telecom_enable_pin 49
#define Ping_PIR_enable_pin 33
#define Camera_enable_pin 35

#define Occupant_thickness 170 //based on Kyron's waist

/*for state machine*/
typedef enum{
	Running, //accelerometer detects movement
	Stopped, //accelerometer detects no movement and no driver detected, determine if state machine needed or go to sleep
	TH0, // No threat
	TH1, // 80 degrees, send notification
	TH2, // 86 degrees, lower windows, 70 ppm
	TH3, // 91 degrees, call 911, 150 ppm
	TH4, // 105 degrees, Open door
}states;

int Occupant_detect(states State);
int Ping_calibration();

#endif /* SKETCH_H_ */