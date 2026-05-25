#include <systemc>
#include <systemc.h>
#include <iostream>
#include "Trafficlightcontroller.h"

using namespace sc_core;

// ============================================================
//  Testbench – covers all slider combinations including
//  illegal transitions (error detection).
// ============================================================
SC_MODULE(Testbench)
{
    sc_out<bool> clk;
    sc_out<bool> sw_blink;
    sc_out<bool> sw_emergency;
    sc_out<bool> sw_next;
    sc_out<bool> sw_priorInParr;
    sc_out<bool> sw_priorInLine;


    SC_CTOR(Testbench) {
        SC_THREAD(stimulus);
    }

    void stimulus() {
        // Initial state
        sw_blink.write(false);
        sw_emergency.write(false);
        sw_next.write(false);
        clk.write(false);
        wait(5, SC_NS);

        auto tick = [&]() {
            clk.write(true);  wait(5, SC_NS);
            clk.write(false); wait(5, SC_NS);
        };

        auto pulse_next = [&]() {
            sw_next.write(true);  tick();
            sw_next.write(false); tick();
        };

        std::cout << "\n=== TEST 1: Normal FSM cycle RED->R+O->GREEN->ORANGE->RED ===" << std::endl;
        tick();           // idle
        pulse_next();     // RED -> RED_ORANGE
        pulse_next();     // RED_ORANGE -> GREEN
        pulse_next();     // GREEN -> ORANGE
        pulse_next();     // ORANGE -> RED
        tick();

        std::cout << "\n=== TEST 2: Enter BLINKING_ORANGE mode ===" << std::endl;
        sw_blink.write(true);  tick();
        tick();
        tick();   // observe 3 blink phases
        sw_blink.write(false); tick();   // return to RED

        std::cout << "\n=== TEST 3: Advance to GREEN then enter EMERGENCY ===" << std::endl;
        pulse_next();     // RED -> RED_ORANGE
        pulse_next();     // RED_ORANGE -> GREEN
        sw_emergency.write(true);  tick();
        tick();
        sw_emergency.write(false); tick();   // return to RED

        std::cout << "\n=== TEST 4: Illegal – press sw_next while in BLINKING_ORANGE (should -> ERROR) ===" << std::endl;
        sw_blink.write(true);  tick();
        pulse_next();           // ILLEGAL → ERROR
        tick();

        std::cout << "\n=== TEST 5: Verify ERROR is a trap state ===" << std::endl;
        pulse_next();    // still ERROR
        sw_blink.write(false); tick();  // still ERROR
        sw_emergency.write(true); tick();
        sw_emergency.write(false); tick();

        std::cout << "\n=== Simulation complete ===" << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[])
{
    // Signals sent by TF controller
    sc_signal<bool> clk;
    sc_signal<bool> sw_blink;
    sc_signal<bool> sw_emergency;
    sc_signal<bool> sw_next;
    sc_signal<bool> sw_priorInParr;
    sc_signal<bool> sw_priorInLine;

    sc_signal<bool> ledA_red, ledA_orange, ledA_green, ledA_blink, ledA_err;
    sc_signal<bool> ledB_red, ledB_orange, ledB_green, ledB_blink, ledB_err;

    /*// Instantiate DUT
    TrafficLightController dut("TrafficLightController");
    dut.clk(clk);
    dut.sw_blink(sw_blink);
    dut.sw_emergency(sw_emergency);
    dut.sw_next(sw_next);
    dut.ledA_red(ledA_red);
    dut.ledA_orange(ledA_orange);
    dut.ledA_green(ledA_green);
    dut.ledA_blink_orange(ledA_blink);
    dut.ledA_error(ledA_err);
    dut.ledB_red(ledB_red);
    dut.ledB_orange(ledB_orange);
    dut.ledB_green(ledB_green);
    dut.ledB_blink_orange(ledB_blink);
    dut.ledB_error(ledB_err); */

    // Instantiate testbench
    Testbench tb("Testbench");
    tb.clk(clk);
    tb.sw_blink(sw_blink);
    tb.sw_emergency(sw_emergency);
    tb.sw_next(sw_next);

    TLController brain;

    /* VVCD trace
    sc_trace_file* tf = sc_create_vcd_trace_file("traffic_light_trace");
    sc_trace(tf, clk,          "clk");
    sc_trace(tf, sw_next,      "sw_next");
    sc_trace(tf, sw_blink,     "sw_blink");
    sc_trace(tf, sw_emergency, "sw_emergency");
    sc_trace(tf, ledA_red,     "A_red");
    sc_trace(tf, ledA_orange,  "A_orange");
    sc_trace(tf, ledA_green,   "A_green");
    sc_trace(tf, ledA_blink,   "A_blink_orange");
    sc_trace(tf, ledA_err,     "A_error");
    sc_trace(tf, ledB_red,     "B_red");
    sc_trace(tf, ledB_orange,  "B_orange");
    sc_trace(tf, ledB_green,   "B_green");
    sc_trace(tf, ledB_blink,   "B_blink_orange");
    sc_trace(tf, ledB_err,     "B_error");
	*/

    sc_start();

    // sc_close_vcd_trace_file(tf);
    return 0;
}




/*
struct Master : sc_module {
    sc_inout<int> bus_port; // Port dwukierunkowy

    SC_CTOR(Master) {
        SC_THREAD(process); // SC_THREAD wymagany do użycia funkcji wait()
    }

    void process() {
        int user_input;

        while (true) {
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

struct Slave : sc_module {
    sc_in<int> bus_port;
    int slave_id;

    SC_HAS_PROCESS(Slave);
    Slave(sc_module_name name, int id) : sc_module(name), slave_id(id) {
        SC_METHOD(read_data);
        sensitive << bus_port; // Metoda uruchomi się ZAWSZE, gdy zmieni się wartość na porcie
        dont_initialize();     // Nie uruchamiaj metody w zerowej sekundzie (przed pierwszym zapisem)
    }

    void read_data() {
        std::cout << "@" << sc_time_stamp() << " [Slave " << slave_id << "] Odczytano dane: " << bus_port.read() << std::endl;
    }
};

int sc_main(int, char*[]) {
    // Współdzielony element reprezentujący magistralę danych
    sc_signal<int> shared_data_bus;

    // Instancjacja modułu zapisująco-odczytującego
    Master master_node("master_node");
    master_node.bus_port(shared_data_bus); // Podłączenie do magistrali

    // Instancjacja dwóch modułów tylko-czytających
    Slave slave_node1("slave_node1", 1);
    slave_node1.bus_port(shared_data_bus); // Podłączenie do tej samej magistrali

    Slave slave_node2("slave_node2", 2);
    slave_node2.bus_port(shared_data_bus); // Podłączenie do tej samej magistrali

    std::cout << "Rozpoczynam symulacje..." << std::endl;

    // Uruchomienie jądra symulacyjnego SystemC na określoną ilość czasu (np. 100 ns)
    sc_start(100, SC_SEC);

    std::cout << "Koniec symulacji." << std::endl;
    return 0;
}*/
