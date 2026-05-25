/*
 * TrafficLight.h
 *
 *  Created on: May 25, 2026
 *      Author: soph
 */

#ifndef TRAFFICLIGHT_H_
#define TRAFFICLIGHT_H_

#include <systemc>
#include <systemc.h>
#include <iostream>

class TrafficLight : public sc_module {
public:
	sc_inout<int> order;

	SC_CTOR(TrafficLight);

	void impulse();
};

#endif /* TRAFFICLIGHT_H_ */
