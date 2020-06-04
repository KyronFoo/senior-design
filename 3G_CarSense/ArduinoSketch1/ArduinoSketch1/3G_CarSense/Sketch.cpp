/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

#include "Adafruit_FONA.h"

#define FONA_RX 2
#define FONA_TX 10
#define FONA_RST 4


char replybuffer[255]; //Buffer for SMS
unsigned long currentMillis = millis();
long previousMillis = 0; //used to store Millis
long interval = 10000; //This is the interval for printing an SMS
int stateInt = 4; //Used to store warning Level

/*
  IN:

  long interval = 10000; Used to store how often automatic warnings are sent
  char warningLevel

  Need a previousMillisdeclaration.


*/

#include <SoftwareSerial.h>
//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio


SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

void setup() {
  while (!Serial);

  Serial.begin(115200);
  //Serial.println(F("FONA SMS caller ID test"));
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

char fonaNotificationBuffer[64];          //for notifications from the FONA
char smsBuffer[250];
//char callerDefault[32]="+18313162043";
char callerDefault[32] = "+14086566470";

void loop() {
  
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
