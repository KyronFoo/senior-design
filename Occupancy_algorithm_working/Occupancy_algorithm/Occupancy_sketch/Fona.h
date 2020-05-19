/*
 * Fona.h
 *
 * Created: 5/1/2020 5:24:57 AM
 *  Author: syngr
 */ 


#ifndef FONA_H_
#define FONA_H_

//#define FONA_RX 2
//#define FONA_TX 3
#define FONA_RST 22

void Fona_setup();
void Fona_SMS_recieve();
bool Fona_Send_sms();


#endif /* FONA_H_ */