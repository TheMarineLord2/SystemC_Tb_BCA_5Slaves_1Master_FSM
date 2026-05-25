/*
 * TrafficLight.cpp
 *
 *  Created on: May 25, 2026
 *      Author: soph
 */

#include "TrafficLight.h"

TrafficLight::SC_CTOR(TrafficLight){
	SC_THREAD(impulse);
}

void TrafficLight::impulse() {

	switch(order){
	case 0: // OFF
		break;
	case 1: // ERROR
		break;
	case 2: // wait
		break;
	case 3: // next
		break;
	case 4: // force green
		break;
	case 5: // force red
		break;

	default:
		std::cout<<"\n Traffic Light == default \n";
		break;
	}
}
