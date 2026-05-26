/*
 * TrafficLight.h
 *
 ** Databus Comms **
	0 = off
	1 = ERROR
	2 = wait
	3 = vehicX
	4 = next()
	5 = vehicI
 */

#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <systemc>
#include <systemc.h>
#include <iostream>


enum TrafficLightState {
	NO_STATE			 = 0,
    STATE_RED            = 1,
    STATE_GREEN   		 = 2,
    STATE_YELLOW         = 3,
    STATE_RED_YELLOW     = 4,
    STATE_BLINKING		 = 5,
	STATE_OFF			 = 7
};

/* When stimulated by CLK:
 * reads 1 int from dataBus
 * resolves command
 * updates lights (prints)
 * */
class TrafficLight : public sc_module {
public:
	sc_in<bool> clk;
	sc_fifo<int> dataBus; //dataBus
	int statePrev;
	int stateCurr;
	// output for traffic lights
	std::string out_vertic;
	std::string out_horiz;

	SC_CTOR(TrafficLight);

	void impulse(); // read data, switch state, continue sending light.
	void showLight(); // vertical is horizontal reversed
};

#endif /* TRAFFICLIGHT_H_ */
