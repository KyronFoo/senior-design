/*
* Fona.cpp
*
* Created: 5/1/2020 5:25:11 AM
*  Author: syngr
*/

#include <Adafruit_FONA.h>
#include "Fona.h"

HardwareSerial *fonaSerial = &Serial1;

Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

char fonaNotificationBuffer[64];          //for notifications from the FONA
char smsBuffer[250];

int slot = 0; 

void Fona_setup(){
	fonaSerial->begin(4800);
	if (! fona.begin(*fonaSerial)) {
		//error
	}
	
	fonaSerial->print("AT+CNMI=2,1\r\n");  //set up the FONA to send a +CMTI notification
}

void Fona_SMS_recieve(){
	
	char* bufPtr = fonaNotificationBuffer;    //handy buffer pointer
	
	if (fona.available())      //any data available from the FONA?
	{
		           //this will be the slot number of the SMS
		int charCount = 0;
		//Read the notification into fonaInBuffer
		do  {
			*bufPtr = fona.read();
			//Serial.write(*bufPtr);
			delay(1);
		} while ((*bufPtr++ != '\n') && (fona.available()) && (++charCount < (sizeof(fonaNotificationBuffer)-1)));
		
		//Add a terminal NULL to the notification string
		*bufPtr = 0;

		//Scan the notification string for an SMS received notification.
		//  If it's an SMS message, we'll get the slot number in 'slot'
		if (1 == sscanf(fonaNotificationBuffer, "+CMTI: " FONA_PREF_SMS_STORAGE ",%d", &slot)) {
			//Serial.print("slot: "); Serial.println(slot);
			
			char callerIDbuffer[32];  //we'll store the SMS sender number in here
			
			// Retrieve SMS sender address/phone number.
			if (! fona.getSMSSender(slot, callerIDbuffer, 31)) {
				//Serial.println("Didn't find SMS message in slot!");
			}
			//Serial.print(F("FROM: ")); Serial.println(callerIDbuffer);

			// Retrieve SMS value.
			uint16_t smslen;
			if (fona.readSMS(slot, smsBuffer, 250, &smslen)) { // pass in buffer and max len!
				//Serial.println(smsBuffer);
			}

		}
	}
}

void Parse_sms(){
	
}

void Fona_Send_sms(){
	char callerIDbuffer[32];
	
	//Send back an automatic response
	//Serial.println("Sending reponse...");
	if (!fona.sendSMS(callerIDbuffer, "Hey, I got your text!")) {
		//Serial.println(F("Failed"));
		} else {
		//Serial.println(F("Sent!"));
	}
	
	// delete the original msg after it is processed
	//   otherwise, we will fill up all the slots
	//   and then we won't be able to receive SMS anymore
	if (fona.deleteSMS(slot)) {
		//Serial.println(F("OK!"));
		} else {
		//Serial.print(F("Couldn't delete SMS in slot ")); Serial.println(slot);
		fona.print(F("AT+CMGD=?\r\n"));
	}
}
