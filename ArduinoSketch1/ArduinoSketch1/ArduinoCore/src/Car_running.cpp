/*
 * Car_running.cpp
 *
 * Created: 1/17/2020 3:40:46 PM
 *  Author: kfoo
 */ 

#include "Substate_machines.h"

enum Car_stopped_states{
	Sleep,
	Poll_sensors,
};

Car_stopped_states Car_stopped_current_state;

top_level_events Car_stopped_sub(){
	top_level_events event = No_event;
	//state machine goes here
	
	switch(Car_stopped_current_state){
		
	}
	
	return event;
}