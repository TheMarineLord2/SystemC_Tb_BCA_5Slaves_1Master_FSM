/*
 * TrafficLight.cpp
 *
 ** Databus States **
	0 = off
	1 = ERROR
	2 = wait
	3 = vehicX
	4 = next()
	5 = vehicI
 */

#include "TrafficLight.h"

TrafficLight::SC_CTOR(TrafficLight){
	        // SC_THREAD(TrafficLight);
	        sensitive << clk.pos();
	    }


int stateNext(int state){
	switch(state){
	case 0: // no state
		return 0; // no state
	case 1: // red
		return 4; // red - yellow
	case 4:	// red - yellow
		return 2; // green
	case 2: // green
		return 3; // yellow
	case 3: // yellow
		return 1; // red
	case 5: // blinking
		return 5; //blinking
	case 7: //off
		return 7;//off
	default:
		return 0;
	}
}

void TrafficLight::impulse() {
	int comms; // = databus get

	switch(comms){
	case 0: // OFF
		stateCurr = 7;
		break;
	case 1: // ERROR
		stateCurr = 6;
		break;
	case 2: // wait
		stateCurr = statePrev;
		break;
	case 3: // vehic X
		stateCurr = 1; // red
		break;
	case 4: // next()
		stateCurr = stateNext(stateCurr);
		break;
	case 5: // vehic I
		stateCurr = 2; // green
		break;
	default:
		stateCurr = 0;
		std::cout<<"\n Traffic Light == break \n";
		break;
	}
	statePrev = stateCurr;

	showLight();
}

void TrafficLight::showLight(){
	switch (stateCurr){
		case 0: // no state
			out_vertic = ' ';
			out_horiz = ' ';
			break;
		case 1: // red
			out_vertic = 'R';
			out_horiz = 'G';
			break;
		case 4:	// red - yellow
			out_vertic = '&';
			out_horiz = 'Y';
			break;
		case 2: // green
			out_vertic = 'G';
			out_horiz = 'R';
			break;
		case 3: // yellow
			out_vertic = 'Y';
			out_horiz = '&';
			break;
		case 5: // blinking
			out_vertic = 'B';
			out_horiz = 'B';
			break;
		case 7: //off
			out_vertic = '_';
			out_horiz = '_';
			break;
		default:
			out_vertic = ' ';
			out_horiz = ' ';
			break;
		}
	std::cout<<"Pion<=["<<out_vertic<<"]["<<out_horiz<<"]=>Przecin\n";
	}

