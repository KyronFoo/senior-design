/*
 * _4G_module.cpp
 *
 * Created: 1/30/2020 2:12:26 AM
 *  Author: syngr
 */ 

#include "Telecom_module.h"
#include "SoftwareSerial.h"
#include "Arduino.h"

int Setup_4G(SoftwareSerial Telecom_serial, int Power_pin){
	int success = 0;
	
	Telecom_serial.begin(9600); //setup telecom serial port

	Serial.println("Initializing..."); //check that USB serial is working and wait for telecom serial port
	delay(1000);
	
	pinMode(Power_pin, OUTPUT); //To turn on module, the power pin must be brought low, then high
	digitalWrite(Power_pin, LOW);
	delay(500);
	digitalWrite(Power_pin, HIGH);
	
	Telecom_serial.println("AT"); //test connection and auto-baud
	updateSerial(Telecom_serial);
	delay(100);
	Telecom_serial.println("ATE1"); //set echo back
	updateSerial(Telecom_serial);
	delay(100);	
	Telecom_serial.println("ATI"); //print module info
	updateSerial(Telecom_serial);
	delay(100);
	Telecom_serial.println("AT+CSQ");//check connection quality
	updateSerial(Telecom_serial);
	delay(100);
	Telecom_serial.println("AT+CREG?");//check network connection
	updateSerial(Telecom_serial);
	delay(100);
	Telecom_serial.println("AT+CICCID");//Read SIM ID
	updateSerial(Telecom_serial);
	delay(100);
	Telecom_serial.println("AT+CSMS?");//check available devices
	updateSerial(Telecom_serial);
	delay(100);
	Telecom_serial.println("AT+CMGF=1");//set module to text mode, set 0 for PDU mode
	updateSerial(Telecom_serial);
	delay(100);
	Telecom_serial.println("AT+CNMI=1,2,0,0,0"); //recieved texts are forwarded to the TE
	updateSerial(Telecom_serial);

	/*sends a text message*/
	//  My_serial.println("AT+CMGS = \"611611\""); //set number for sms
	//  updateSerial();
	//  My_serial.print("USAGE"); //sms content
	//  updateSerial();
	//  My_serial.write(26); //send ctrl-z to terminate text and send
	
	return success;
}

//void updateSerial(SoftwareSerial Telecom_serial){
	//delay(500);
	//while (Serial.available()) {
		//Telecom_serial.println(Serial.read());
	//}
	//while(Telecom_serial.available()){
		//Serial.write(Telecom_serial.read());
	//}
//}