/*
 * Trafficlightcontroller.h
 *
 *  Created on: May 25, 2026
 *      Author: soph
 */

#ifndef TRAFFICLIGHTCONTROLLER_H
#define TRAFFICLIGHTCONTROLLER_H

#include <systemc>
#include <systemc.h>
#include <iostream>
#include "TrafficLight.h"

class Controller : sc_module {
	sc_in<bool> sw_autom;
	sc_in<bool> sw_blink;
	sc_in<bool> sw_next;
	sc_in<bool> sw_priorInParr;
	sc_in<bool> sw_priorInLine;


};

class TLController : public TrafficLight {
public:
	// Switches in to controll
	/*sc_in<bool> sw_autom;
	sc_in<bool> sw_blink;
	sc_in<bool> sw_next;
	sc_in<bool> sw_priorInParr;
	sc_in<bool> sw_priorInLine;*/

	// Controller controllerMod;;
};


#endif /* TRAFFICLIGHTCONTROLLER_H_ */
