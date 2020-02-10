/*
 * _4G_module.h
 *
 * Created: 1/30/2020 2:11:33 AM
 *  Author: syngr
 */ 


#ifndef TELECOM_MODULE_H_
#define TELECOM_MODULE_H_

#include "SoftwareSerial.h"


int Setup_4G(SoftwareSerial Telecom_serial, int Power_pin);
//void updateSerial(SoftwareSerial Telecom_serial); //function to echo communications through the Uno


#endif /* 4G_MODULE_H_ */