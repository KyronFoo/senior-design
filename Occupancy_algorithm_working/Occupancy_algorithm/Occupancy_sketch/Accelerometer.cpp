/*
 * CPPFile1.cpp
 *
 * Created: 4/29/2020 9:35:38 PM
 *  Author: syngr
 */ 

#include "Accelerometer.h"
#include <Wire.h>

long accelX, accelY, accelZ;      // stores data read from accelerometer on MPU6050
float gForceX, gForceY, gForceZ;  // stores G forces acting in 3 directions

long gyroX, gyroY, gyroZ;         // stores data read from gyroscope read on MPU6050
float rotX, rotY, rotZ;

MPU_data Read_data;


void setupMPU(){  //purpose: establish communication with MPU and set up registers
	Wire.beginTransmission(MPU_Address); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
	Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
	Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
	Wire.endTransmission();
	Wire.beginTransmission(MPU_Address); //I2C address of the MPU
	Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4)
	Wire.write(0x00000000); //Setting the gyro to full scale +/- 250deg./s
	Wire.endTransmission();
	Wire.beginTransmission(MPU_Address); //I2C address of the MPU
	Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5)
	Wire.write(0b00011000); //Setting the accel to +/- 2g, +/-16 would be Wire.write(0b00011000), +/- 4 would be (0b00011000)
	Wire.endTransmission();
}

void processAccelData(){
	Read_data.gForceX = accelX / Accel_Scale;
	Read_data.gForceY = accelY / Accel_Scale;
	Read_data.gForceZ = accelZ / Accel_Scale;
}

void recordAccelRegisters() {
	Wire.beginTransmission(MPU_Address); //I2C address of the MPU
	Wire.write(0x3B); //Starting register for Accel Readings
	Wire.endTransmission();
	Wire.requestFrom(MPU_Address,6); //Request Accel Registers (3B - 40)
	while(Wire.available() < 6);
	accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
	accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
	accelZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
	processAccelData();
}

void processGyroData() {
	Read_data.rotX = gyroX / Gyro_Scale;
	Read_data.rotY = gyroY / Gyro_Scale;
	Read_data.rotZ = gyroZ / Gyro_Scale;
}

void recordGyroRegisters() {
	Wire.beginTransmission(MPU_Address); //I2C address of the MPU
	Wire.write(0x43); //Starting register for Gyro Readings
	Wire.endTransmission();
	Wire.requestFrom(MPU_Address,6); //Request Gyro Registers (43 - 48)
	while(Wire.available() < 6);
	gyroX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
	gyroY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
	gyroZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
	processGyroData();
}


//void printData() {
	//Serial.print("Gyro (deg)");
	//Serial.print(" X=");
	//Serial.print(rotX);
	//Serial.print(" Y=");
	//Serial.print(rotY);
	//Serial.print(" Z=");
	//Serial.print(rotZ);
	//Serial.print(" Accel (g)");
	//Serial.print(" X=");
	//Serial.print(gForceX); 
	//Serial.print(" Y=");
	//Serial.print(gForceY);
	//Serial.print(" Z=");
	//Serial.println(gForceZ);
//}

MPU_data Get_MPU_Data(){
	MPU_data data;
	recordAccelRegisters();
	recordGyroRegisters();
	data = Read_data;
	return data;
}
