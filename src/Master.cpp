/*
 * Master.cpp
 *
 *  Created on: May 21, 2026
 *      Author: soph
 */
#include <systemc>
#include <iostream>
#include "Master.h"
using namespace sc_core;

struct Master : sc_module {
    sc_inout<int> bus_port; // Port dwukierunkowy

    SC_CTOR(Master) {
        SC_THREAD(process); // SC_THREAD wymagany do użycia funkcji wait()
    }

    void process() {
        int user_input;

        while (true) {
        	// Kolejność stanów jako
            std::cout << "\n@" << sc_time_stamp()
                      << " [Master] Podaj liczbe do wyslania na magistrale (lub -1 aby zakonczyc): ";

            // Pobieranie danych z klawiatury.
            // UWAGA: Czas symulacji "stoi w miejscu" dopóki użytkownik nie wciśnie Enter.
            std::cin >> user_input;

            // Warunek wyjścia z pętli i zakończenia symulacji
            if (user_input == -1) {
                std::cout << "@" << sc_time_stamp() << " [Master] Zamykanie systemu..." << std::endl;
                sc_stop(); // Zatrzymuje jądro symulacyjne SystemC
                break;
            }

            // 1. Zapis na magistralę
            bus_port.write(user_input);
            std::cout << "@" << sc_time_stamp() << " [Master] Zapisano dane: " << user_input << std::endl;

            // Czekamy 1 sekundę symulacyjną na propagację danych
            wait(1, SC_SEC);

            // 2. Odczyt z magistrali (potwierdzenie własnego zapisu)
            std::cout << "@" << sc_time_stamp() << " [Master] Odczytano wlasne dane: " << bus_port.read() << std::endl;

            // Czekamy kolejne 2 sekundy symulacyjne przed kolejnym zapytaniem
            wait(2, SC_SEC);
        }
    }
};
