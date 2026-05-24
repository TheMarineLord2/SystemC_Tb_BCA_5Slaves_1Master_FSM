/*
 * Master.h
 *
 *  Created on: May 21, 2026
 *      Author: soph
 */

#ifndef MASTER_H_
#define MASTER_H_

#include <systemc>
#include <iostream>
#include "Master.h"
using namespace sc_core;

struct Master : sc_module {
	// controll module
    sc_inout<int> bus_port;

    SC_CTOR(Master);

    void process();
};


#endif /* MASTER_H_ */
