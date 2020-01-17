/*
 * Critical_1.cpp
 *
 * Created: 1/17/2020 3:42:17 PM
 *  Author: kfoo
 */ 

#include "Substate_machines.h"

enum Critical_1_states{
	Sleep,
	Check_for_cond,
	Notify_owner,
	Wait_for_response,
	Roll_down_windows
};

Critical_1_states Critical_1_current_state;

top_level_events Critical_1_sub(){
	top_level_events event = No_event;
	//state machine goes here
	switch (Critical_1_current_state){
		
	}
	return event;
}
