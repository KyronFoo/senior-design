/*
 * Camera.h
 *
 * Created: 2/9/2020 10:04:58 PM
 *  Author: syngr
 */ 


#ifndef CAMERA_H_
#define CAMERA_H_

#include <Adafruit_AMG88xx.h>
#include "Pins.h"

void Camera_setup(Adafruit_AMG88xx amg);
void Camera_read(Adafruit_AMG88xx amg, float *pixels);

#endif /* CAMERA_H_ */