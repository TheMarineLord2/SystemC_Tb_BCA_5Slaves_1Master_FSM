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


enum TrafficState {
    STATE_RED            = 0,   // Czerwony
    STATE_RED_ORANGE     = 1,   // Czerwony + Pomarańczowy
    STATE_GREEN          = 2,   // Zielony
    STATE_ORANGE         = 3,   // Pomarańczowy
    STATE_BLINKING_ORANGE= 4,   // Pomarańczowy migający
    STATE_VEHICX	     = 5,   // Przejazd pojazdu uprzywilejowanego
    STATE_VEHICP         = 6,
	STATE_ERROR			 = 7
};

class TLController : public sc_module {
public:
	// switches for manual control:
	sc_in<bool> clk;
	sc_out<int> data; //dataBus
	/** Databus States **/
	//0 = off
	//1 = ERROR
	//2 = wait
	//3 = vehicX
	//5 = next()
	//6 = vehicP
	//7 = automatic.

	// Man Controll:
	sc_in<bool> sw_autom;
	sc_in<bool> sw_blink;
	sc_in<bool> sw_next;
	sc_in<bool> sw_priorInParr;
	sc_in<bool> sw_priorInLine;

	SC_CTOR(TLController) {
		SC_THREAD(impulse);
	}

	void impulse();
};

#endif /* TRAFFICLIGHTCONTROLLER_H_ */
