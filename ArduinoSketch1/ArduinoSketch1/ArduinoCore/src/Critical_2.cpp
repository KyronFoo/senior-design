/*
 * CPPFile1.cpp
 *
 * Created: 1/17/2020 3:43:21 PM
 *  Author: kfoo
 */ 

#include "Substate_machines.h"

enum Critical_2_states{
	Sleep,
	Check_for_cond,
	Notify_owner,
	Wait,
	Notify_EMS
};

Critical_2_states Critical_2_current_state;

top_level_events Critical_2_sub(){
	top_level_events event = No_event;
	//state machine goes here
	switch(Critical_2_current_state){
		
	}
	return event;
}