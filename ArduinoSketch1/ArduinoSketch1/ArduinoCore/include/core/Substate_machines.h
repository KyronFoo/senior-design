/*
* Substate_machines.h
*
* Created: 1/17/2020 1:30:06 PM
*  Author: kfoo
*/


#ifndef SUBSTATE-MACHINES_H_
#define SUBSTATE-MACHINES_H_

#define Car_running_waiting_for_movement_time 5000 //5000 is a placeholder, actual time to be determined later 1/17/20
#define Car_running_sleep_time 5000 //5000 is a placeholder, actual time to be determined later 1/17/20
#define Car_stopped_sleep_time 5000 //5000 is a place holder 1/17/20
#define Critical_1_sleep_time 5000 //5000 is a place holder 1/17/20
#define Critical_1_wait_time 5000 //5000 is a place holder 1/17/20
#define Critical_2_wait_time 5000 //5000 is a place holder 1/17/20
#define Critical_2_sleep_time 5000 //5000 is a place holder 1/17/20
#define Critical_3_sleep_time 5000 //5000 is a place holder 1/17/20
#define Critical_3_wait_time 5000 //5000 is a place holder 1/17/20

enum top_level_events{
	Occupancy_detected,
	Owner_dismissed,
	Accelerometer_in_motion,
	Accelerometer_stopped,
	Go_to_critical_2,
	Go_to_critical_3,
	No_event
};

//enum events{
	//Timeout,
	//Con_1_true,
	//Con_2_true,
	//Con_3_true,
	//No_owner_response,
	//No_event
//};



top_level_events Car_running_sub();
top_level_events Car_stopped_sub();
top_level_events Critical_1_sub();
top_level_events Critical_2_sub();
top_level_events Critical_3_sub();


#endif /* SUBSTATE-MACHINES_H_ */