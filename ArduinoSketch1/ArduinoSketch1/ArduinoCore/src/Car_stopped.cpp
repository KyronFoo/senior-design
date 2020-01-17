/*
* Substate_machines.cpp
*
* Created: 1/17/2020 1:36:05 PM
*  Author: kfoo
*/

#include "Substate_machines.h"

enum Car_running_states{
	Sleep, //power saving state
	Poll_accelerometer, //check for car movement
	Waiting_for_movement //wait for red-light time to pass, maybe install check for occupancy and con 1
};

Car_running_states Car_running_current_state;

top_level_events Car_running_sub(){
	top_level_events event = No_event;
	//state machine goes here
	
	switch(Car_running_current_state){
		
	}
	
	return event;
}







