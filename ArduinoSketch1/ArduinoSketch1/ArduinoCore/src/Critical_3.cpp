/*
 * Critical_3.cpp
 *
 * Created: 1/17/2020 3:44:26 PM
 *  Author: kfoo
 */ 

#include "Substate_machines.h"

enum Critical_3_states{
	Sleep,
	Check_for_cond,
	Notify_owner,
	Wait_for_response,
	Open_doors,
	Continue_notification
};

Critical_3_states Critical_3_current_state;

top_level_events Critical_3_sub(){
	top_level_events event = No_event;
	//state machine goes here
	switch(Critical_3_current_state){
		
	}
	return event;
}
