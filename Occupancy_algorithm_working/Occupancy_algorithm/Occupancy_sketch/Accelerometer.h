/*
 * Accelerometer.h
 *
 * Created: 4/29/2020 9:30:50 PM
 *  Author: syngr
 */ 


#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#define MPU_Address 0b1101000
#define Accel_Scale 2048.0
#define Gyro_Scale 164.0

struct MPU_data
{
	float gForceX, gForceY, gForceZ, rotX, rotY, rotZ;	
};

MPU_data Get_MPU_Data(); 
void setupMPU();

#endif /* ACCELEROMETER_H_ */