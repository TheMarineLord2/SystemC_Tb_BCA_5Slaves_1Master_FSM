/*
 * TrafficLight.cpp
 *
 *  Created on: May 21, 2026
 *      Author: soph
 */


#include <systemc>
#include <iostream>
#include "TrafficLight.h"
using namespace sc_core;

struct TrafficLight : sc_module {
    sc_in<int> bus_port;
    int slave_id;

    SC_HAS_PROCESS(Slave);
    TrafficLight(sc_module_name name, int id) : sc_module(name), slave_id(id) {
        SC_METHOD(read_data);
        sensitive << bus_port; // Metoda uruchomi się ZAWSZE, gdy zmieni się wartość na porcie
        dont_initialize();     // Nie uruchamiaj metody w zerowej sekundzie (przed pierwszym zapisem)
    }

    void read_data() {
        std::cout << "@" << sc_time_stamp() << " [Slave " << slave_id << "] Odczytano dane: " << bus_port.read() << std::endl;
    }
};
