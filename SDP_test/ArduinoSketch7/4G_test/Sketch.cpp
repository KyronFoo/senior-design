/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

#include <hcsr04.h> //ultrasonic rangefinder library
#include <Adafruit_AMG88xx.h> //infrared camera library
#include <SoftwareSerial.h> //Serial communication library
#include <Wire.h> //Used by infrared camera library for I2C communication
//#include "Telecom_module.h" //4g helper functions



Adafruit_AMG88xx amg; //define thermal camera
float pixels[AMG88xx_PIXEL_ARRAY_SIZE]; //create array for incoming pixels

#define Telecom_TX 50 //connects to rx pin on telecom module
#define Telecom_RX 51 //connects to tx pin on telecom module
#define Power_pin 53 //connects to power pin on telecom module
#define TRIG_PIN 4
#define ECHO_PIN 5
#define PIR_read_pin 10
//#define Telecom_enable_pin 49
#define Ping_PIR_enable_pin 11
#define Camera_enable_pin 9


SoftwareSerial Telecom_serial(Telecom_RX, Telecom_TX); //set up serial port for telecom module
HCSR04 hcsr04(TRIG_PIN, ECHO_PIN, 20, 4000); //setup ping sensor

void Enable_pin_init();
void Camera_setup();
void Camera_read();
void PIR_setup();
int Setup_4G();

void updateSerial() {
	delay(500);
	while (Serial.available()) {
		Telecom_serial.println(Serial.read());
	}
	while (Telecom_serial.available()) {
		Serial.write(Telecom_serial.read());
	}
}

void setup() {
	// put your setup code here, to run once:
	int Initial_time = millis();
	
	Serial.begin(9600); //setup USB
	while (!Serial);
	
	Enable_pin_init();
	
	Setup_4G(); //setup telecom unit
	
	Camera_setup(); //setup IR camera
	
	PIR_setup(); //setup PIR pin
	
	delay (100);
	
	Camera_read(); //IR camera read
	
	digitalWrite(Ping_PIR_enable_pin, HIGH); //turn on sensors for read
	delay(100);
	Serial.println(hcsr04.ToString()); //read ping sensor
	if( digitalRead(PIR_read_pin) != 0){ //read PIR sensor
		Serial.println("PIR positive detect");
	}
	else {
		Serial.println("PIR negative detect");
	}
	
	int Setup_time = millis() - Initial_time;
	
	Serial.print("Setup time: ");
	Serial.println(Setup_time);
	
	digitalWrite(Ping_PIR_enable_pin, LOW);
}

void loop() {
	// put your main code here, to run repeatedly:
	updateSerial(); //Telecom module read
	
	//Camera_read(); //IR camera read
	
	//Serial.println(hcsr04.ToString()); //read ping sensor
	
	//Serial.println(digitalRead(PIR_read_pin)); //read PIR sensor
}

int Setup_4G(){
	int success = 0;
	
	Telecom_serial.begin(9600); //setup telecom serial port

	Serial.println("Initializing..."); //check that USB serial is working and wait for telecom serial port
	delay(1000);
	
	pinMode(Power_pin, OUTPUT); //To turn on module, the power pin must be brought low, then high
	digitalWrite(Power_pin, LOW);
	delay(500);
	digitalWrite(Power_pin, HIGH);
	
	Telecom_serial.println("AT"); //test connection and auto-baud
	updateSerial();
	delay(100);
	Telecom_serial.println("ATE1"); //set echo back
	updateSerial();
	delay(100);
	Telecom_serial.println("ATI"); //print module info
	updateSerial();
	delay(100);
	Telecom_serial.println("AT+CSQ");//check connection quality
	updateSerial();
	delay(100);
	Telecom_serial.println("AT+CREG?");//check network connection
	updateSerial();
	delay(100);
	Telecom_serial.println("AT+CICCID");//Read SIM ID
	updateSerial();
	delay(100);
	Telecom_serial.println("AT+CSMS?");//check available devices
	updateSerial();
	delay(100);
	Telecom_serial.println("AT+CMGF=1");//set module to text mode, set 0 for PDU mode
	updateSerial();
	delay(100);
	Telecom_serial.println("AT+CNMI=1,2,0,0,0"); //recieved texts are forwarded to the TE
	updateSerial();
	
	Telecom_serial.println("AT+CGPS = 1");
	updateSerial();

	/*sends a text message*/
	//Telecom_serial.println("AT+CMGS = \"+15105996769\""); //set number for sms
	//updateSerial();
	//	My_serial.print("USAGE"); //sms content
	//Telecom_serial.print("Arduino texting you");
	//updateSerial();
	//Telecom_serial.write(26); //send ctrl-z to terminate text and send
	
	return success;
}

void Enable_pin_init(){
	//pinMode(Telecom_enable_pin, OUTPUT);
	pinMode(Ping_PIR_enable_pin, OUTPUT);
	pinMode(Camera_enable_pin, OUTPUT);
	//digitalWrite(Telecom_enable_pin, HIGH);
	digitalWrite(Ping_PIR_enable_pin, LOW);
	digitalWrite(Camera_enable_pin, LOW);
}

void Camera_setup(){
	bool status;
	
	digitalWrite(Camera_enable_pin, HIGH);
	delay(100);
	
	// default settings
	status = amg.begin();
	if (!status) {
		Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
		while (1);
	}
	
	Serial.println("-- Pixels Test --");

	Serial.println();

	delay(100); // let sensor boot up
}

void Camera_read(){
	//read all the pixels
	amg.readPixels(pixels);

	Serial.print("[");
	for(int i=1; i<=AMG88xx_PIXEL_ARRAY_SIZE; i++){
		Serial.print(pixels[i-1]);
		Serial.print(", ");
		if( i%8 == 0 ) Serial.println();
	}
	Serial.println("]");
	Serial.println();
	
	digitalWrite(Camera_enable_pin, LOW);

	//delay a second
	delay(1000);
}

void PIR_setup(){
	pinMode(PIR_read_pin, INPUT);
}


