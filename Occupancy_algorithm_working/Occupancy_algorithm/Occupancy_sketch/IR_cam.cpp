/*
* TFT_screen.cpp
*
* Created: 3/6/2020 11:07:34 PM
*  Author: kfoo
*/

#include "IR_cam.h"


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

	//tft.initR(INITR_144GREENTAB);   // initialize a ST7735S chip, black tab
	//tft.fillScreen(ST7735_BLACK);

	//displayPixelWidth = tft.width() / 8;
	//displayPixelHeight = tft.height() / 8;

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

//int Print_raw_data = false;
//int Temp_print_delay;

Thermal_SizeTemp_Struct Thermal_read(){
	
	amg.readPixels(pixels);
	struct Thermal_SizeTemp_Struct Size_temp_read;
	//int cold = 40;
	Size_temp_read.max_temp = 0;
	Size_temp_read.size = 0;
	Size_temp_read.detected = 0;

	//Parse raw reads for info
	
	//Temp_print_delay++;
	//
	//if (Temp_print_delay >= 25){
		//Print_raw_data = true;
		//Temp_print_delay = 0;
		//} 
		//if(Print_raw_data == true) {
		//Serial.print("[");
		//for(int i=1; i<=AMG88xx_PIXEL_ARRAY_SIZE; i++){
			//Serial.print(pixels[i-1]);
			//Serial.print(", ");
			//if( i%8 == 0 ) Serial.println();
		//}
		//Serial.println("]");
		//Serial.println();
		//Print_raw_data = false;
	//}

	
	for(int i=0; i<AMG88xx_PIXEL_ARRAY_SIZE; i++){
		if(pixels[i] > Size_temp_read.max_temp){
			Size_temp_read.max_temp = pixels[i];
		}
		if(pixels[i] >= DETECTEMP){
			Size_temp_read.size++;
		}
	}
	
	if ((Size_temp_read.max_temp > DETECTEMP) && (Size_temp_read.size > 4)){
		Size_temp_read.detected = 1;
	}
	
	////modify array for display
	//for(int i=0; i<AMG88xx_PIXEL_ARRAY_SIZE; i++){
		//if(pixels[i] > MAXTEMP){
			//pixels[i] = MAXTEMP; //prevent roll-over back to blue
		//}
		//uint8_t colorIndex = map(pixels[i], MINTEMP, MAXTEMP, 0, 255);
		//colorIndex = constrain(colorIndex, 0, 255);
//
		////draw the pixels!
		//tft.fillRect(displayPixelHeight * floor(i / 8), displayPixelWidth * (i % 8),
		//displayPixelHeight, displayPixelWidth, camColors[colorIndex]);
	//}
	
	
	
	return Size_temp_read;
}

