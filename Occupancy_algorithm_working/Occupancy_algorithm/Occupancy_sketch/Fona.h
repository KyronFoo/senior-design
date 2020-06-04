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
bool Fona_Send_sms(int Felt_temp, int Humidity, int C_O, bool Occupant_flag, float latitude, char N_S, float longitude, char E_W);


#endif /* FONA_H_ */