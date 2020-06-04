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
  Serial.println(F("Initializing...."));

  fonaSerial->begin(4800); //Set baud rate to 4800
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("Couldn't find FONA"));
    while (1);
  }
  Serial.println(F("FONA is CONNECTED"));

  /*
    // Print SIM card IMEI number.
    char imei[16] = {0}; // MUST use a 16 character buffer for IMEI!
    uint8_t imeiLen = fona.getIMEI(imei);
    if (imeiLen > 0) {
    Serial.print("SIM card IMEI: "); Serial.println(imei);
    }
  */
  fonaSerial->print("AT+CNMI=2,1\r\n");  //set up the FONA to send a +CMTI notification when an SMS is received

  Serial.println("FONA Ready");
}

int stateInt = 4; //Used to store warning Level

void Fona_SMS_recieve(){
	
	char* bufPtr = fonaNotificationBuffer;    //handy buffer pointer

  if (fona.available())      //any data available from the FONA?
  {

    int slot = 0;            //this will be the slot number of the SMS
    int charCount = 0;
    char callerIDbuffer[32];  //we'll store the SMS sender number in here
    //Read the notification into fonaInBuffer
    do  {
      *bufPtr = fona.read();
      Serial.write(*bufPtr);
      delay(1);
    } while ((*bufPtr++ != '\n') && (fona.available()) && (++charCount < (sizeof(fonaNotificationBuffer) - 1)));

    //Add a terminal NULL to the notification string
    *bufPtr = 0;

    //Scan the notification string for an SMS received notification.
    //  If it's an SMS message, we'll get the slot number in 'slot'
    if (1 == sscanf(fonaNotificationBuffer, "+CMTI: " FONA_PREF_SMS_STORAGE ",%d", &slot)) {
      Serial.print("slot: "); Serial.println(slot);



      // Retrieve SMS sender address/phone number.
      if (! fona.getSMSSender(slot, callerIDbuffer, 31)) {
        Serial.println("Didn't find SMS message in slot!");
      }
      Serial.print(F("FROM: ")); Serial.println(callerIDbuffer);

      // Retrieve SMS value.
      uint16_t smslen;
      if (fona.readSMS(slot, smsBuffer, 250, &smslen)) { // pass in buffer and max len!
        Serial.println(smsBuffer);
      }

      //Send back an automatic response
      Serial.println("Sending response...");
      //if (!fona.sendSMS(callerIDbuffer, "Confirmation")) {

      char level[32] = "TH1 = ";
      char state[32];

      sprintf(state, "%d, ", stateInt);
      char Heat[32] = "Heat Index = ";


      strcat(level, state);
      //s.concat(3);
      if (!fona.sendSMS(callerIDbuffer, level)) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("Sent!"));
      }
	  
	  delay(1000);

      // delete the original msg after it is processed
      //   otherwise, we will fill up all the slots
      //   and then we won't be able to receive SMS anymore
      if (fona.deleteSMS(slot)) {
        Serial.println(F("OK!"));
      } else {
        Serial.print(F("Couldn't delete SMS in slot ")); Serial.println(slot);
        fona.print(F("AT+CMGD=?\r\n"));
      }
    }

  }
  /*
     //This is the loop
    if(currentMillis-previousMillis>interval){
      Serial.println("10 seconds passed");
      if(state = 0)
      {
      previousMillis=currentMillis;
     // fona.sendSMS(callerDefault,"TH = 1");
      }
    }
  */

}

void Parse_sms(){
	
}

bool Fona_Send_sms(){ //need to be programmed with numbers
	char callerIDbuffer[32];
	
	  Serial.println("Sending reponse...");
	  //if (!fona.sendSMS(callerIDbuffer, "Confirmation")) {

	  char level[32] = "TH1 = ";
	  char state[32];

	  sprintf(state, "%d, ", stateInt);
	  char Heat[32] = "Heat Index = ";


	  strcat(level, state);
	  //s.concat(3);
	  if (!fona.sendSMS(callerIDbuffer, level)) {
		  Serial.println(F("Failed"));
		  } else {
		  Serial.println(F("Sent!"));
	  }

	  // delete the original msg after it is processed
	  //   otherwise, we will fill up all the slots
	  //   and then we won't be able to receive SMS anymore
	  if (fona.deleteSMS(slot)) {
		  Serial.println(F("OK!"));
		  } else {
		  Serial.print(F("Couldn't delete SMS in slot ")); Serial.println(slot);
		  fona.print(F("AT+CMGD=?\r\n"));
	  }
}
