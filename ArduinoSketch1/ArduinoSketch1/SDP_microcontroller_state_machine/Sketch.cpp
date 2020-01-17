/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>
#include "Substate_machines.h"

/*End of auto generated code by Atmel studio */


//Beginning of Auto generated function prototypes by Atmel Studio
//End of Auto generated function prototypes by Atmel Studio


enum states {
    Car_running,
    Car_stopped,
    Critical_1,
    Critical_2,
    Critical_3
  };

  states current_state = Car_stopped;

void setup() {
  // put your setup code here, to run once: 

}

top_level_events event = No_event;

void loop() {
  // put your main code here, to run repeatedly:
  switch(current_state){
    case Car_stopped:
	//run substate machine here
	event = Car_stopped_sub();
    break;
    
    case Car_running:
	event = Car_running_sub();
    break;
    
    case Critical_1:
	event = Critical_1_sub();
    break;
    
    case Critical_2:
	event = Critical_2_sub();
    break;
    
    case Critical_3:
	event = Critical_3_sub();
    break;
  }
}
