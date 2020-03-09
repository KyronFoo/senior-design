/*
* TFT_screen.cpp
*
* Created: 3/6/2020 11:07:34 PM
*  Author: kfoo
*/

#include "TFT_screen.h"


uint16_t displayPixelWidth, displayPixelHeight;
float pixels[AMG88xx_PIXEL_ARRAY_SIZE]; //create array for incoming pixels
unsigned long delayTime;
/*
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);
*/

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

Adafruit_AMG88xx amg;



int Thermal_setup(){
	//Serial.println(F("AMG88xx thermal camera!"));

	tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab
	tft.fillScreen(ST7735_BLACK);

	displayPixelWidth = tft.width() / 8;
	displayPixelHeight = tft.height() / 8;

	//tft.setRotation(3);
	
	bool status;
	
	// default settings
	status = amg.begin();
	if (!status) {
		Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
		while (1);
	}
	
	Serial.println("-- Thermal Camera Test --");
	delay(100); // let sensor boot up

}


Thermal_SizeTemp_Struct Thermal_read(){
	amg.readPixels(pixels);
	struct Thermal_SizeTemp_Struct Size_temp_read;
	//int cold = 40;

	//Parse raw reads for info

	for(int i=0; i<AMG88xx_PIXEL_ARRAY_SIZE; i++){
		if(pixels[i] > Size_temp_read.high_temp){
			Size_temp_read.high_temp = pixels[i];
		}
		if(pixels[i] >= MAXTEMP - 2){
			Size_temp_read.size++;
		}

	}
	//modify array for display
	for(int i=0; i<AMG88xx_PIXEL_ARRAY_SIZE; i++){
		if(pixels[i] > MAXTEMP){
			pixels[i] = MAXTEMP; //prevent roll-over back to blue
		}
		uint8_t colorIndex = map(pixels[i], MINTEMP, MAXTEMP, 0, 255);
		colorIndex = constrain(colorIndex, 0, 255);

		//draw the pixels!
		tft.fillRect(displayPixelHeight * floor(i / 8), displayPixelWidth * (i % 8),
		displayPixelHeight, displayPixelWidth, camColors[colorIndex]);
	}

	int size;
	
	
	
	return Size_temp_read;
}

