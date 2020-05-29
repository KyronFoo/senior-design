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
		Serial.print("[");
		for(int i=1; i<=AMG88xx_PIXEL_ARRAY_SIZE; i++){
			Serial.print(pixels[i-1]);
			Serial.print(", ");
			if( i%8 == 0 ) Serial.println();
		}
		Serial.println("]");
		Serial.println();
		//Print_raw_data = false;
	//}

	//search for the max temperature
	for(int i=0; i<AMG88xx_PIXEL_ARRAY_SIZE; i++){
		if(pixels[i] > Size_temp_read.max_temp){
			Size_temp_read.max_temp = pixels[i];
		}
		if(pixels[i] >= DETECTEMP){
			Size_temp_read.size++;
		}
	}
	
	//parse data for sunlight
	//parse left side of the image. In this case the top half of the image. 
	
	int Left_average_temperature;
	
	for(int i=0; i<(AMG88xx_PIXEL_ARRAY_SIZE/2); i++){
		Left_average_temperature += pixels[i]; //sum up temperatures on the top half of the image
	}
	Left_average_temperature = Left_average_temperature / (AMG88xx_PIXEL_ARRAY_SIZE/2);
	
	int Right_average_temperature;
	
	for (int i = AMG88xx_PIXEL_ARRAY_SIZE/2; i<AMG88xx_PIXEL_ARRAY_SIZE; i++)
	{
		Right_average_temperature += pixels[i]; //sum up temperatures on the bottom half of the image
	}
	Right_average_temperature = Right_average_temperature / (AMG88xx_PIXEL_ARRAY_SIZE/2); 
	
	//if the two sides are unequal in temperature, we likely have sunlight
	if ((Left_average_temperature - Right_average_temperature) > 3)
	{
		//flag sunlight
	}
	
	//parse data for occupant
	int top_temps[5]; 
	int j = 0; //index for top temps
	int pixels_sum;
	top_temps[0] =  Size_temp_read.max_temp; //save max temp to start of array
	
	//find top 5 temperatures in the array
	for (j=1; j<5; j++){
		for(int i=0; i <AMG88xx_PIXEL_ARRAY_SIZE; i++){
			if((pixels[i] > top_temps[j]) && (pixels[i] < top_temps[j-1])){ //each consecutive value will be lower than before
				top_temps[j] = pixels[i];
			} else { //otherwise, add it to the average sum
				pixels_sum = pixels_sum + pixels[i];
			}
		}
	}
	//calculate the average temperature excluding the 5 highest temperatures
	int average_temperature = pixels_sum / (AMG88xx_PIXEL_ARRAY_SIZE - 5);
	
	if ((Size_temp_read.max_temp - 2) > average_temperature){ //check if hottest pixel is hotter than the background temperature by 2 celcius
		Size_temp_read.detected = true;
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

