#include <systemc>
#include <iostream>
#include <string>

using namespace sc_core;

// ============================================================
// State Machine & Helpers
// ============================================================
enum LightState {
    STATE_RED           = 1,
    STATE_RED_YELLOW    = 4,
    STATE_GREEN         = 2,
    STATE_YELLOW        = 3,
    STATE_BLINKING      = 5,
    STATE_ERROR         = 6,
    STATE_OFF           = 7
};

int nextState(int current) {
    switch(current) {
        case STATE_RED:        return STATE_RED_YELLOW;
        case STATE_RED_YELLOW: return STATE_GREEN;
        case STATE_GREEN:      return STATE_YELLOW;
        case STATE_YELLOW:     return STATE_RED;
        case STATE_BLINKING:   return STATE_BLINKING;
        case STATE_ERROR:      return STATE_ERROR;
        case STATE_OFF:        return STATE_OFF;
        default:               return STATE_RED;
    }
}

void displayLight(int state, int controller_id) {
    std::string v, h;
    switch(state) {
        case STATE_RED:        v = "R";   h = "G";   break;
        case STATE_RED_YELLOW: v = "R+O"; h = "Y";   break;
        case STATE_GREEN:      v = "G";   h = "R";   break;
        case STATE_YELLOW:     v = "Y";   h = "R+O"; break;
        case STATE_BLINKING:   v = "B";   h = "B";   break;
        case STATE_ERROR:      v = "E";   h = "E";   break;
        case STATE_OFF:        v = "_";   h = "_";   break;
        default:               v = "?";   h = "?";
    }
    std::cout << "  [ID: " << controller_id << "][" << v << "][" << h << "] \n";
}

// ============================================================
// Slave: Traffic Light Controller (BCA Synchronized)
// ============================================================
SC_MODULE(TrafficLightController) {
    sc_in<bool> clk;

    // Interfejs magistrali (Slave)
    sc_in<int>  bus_cmd;
    sc_in<bool> bus_req;
    sc_out<bool> bus_ack;

    int controller_id;
    int stateCurr;
    int statePrev;

    SC_HAS_PROCESS(TrafficLightController);

    TrafficLightController(sc_module_name name, int id)
        : sc_module(name), controller_id(id), stateCurr(STATE_RED), statePrev(STATE_RED) {
        SC_METHOD(clockHandler);
        sensitive << clk.pos(); // Reakcja wyłącznie na zbocze narastające
        dont_initialize();
    }

    void clockHandler() {
        // Obsługa protokołu magistrali (Handshake)
        if (bus_req.read() == true && bus_ack.read() == false) {
            int cmd = bus_cmd.read();

            switch(cmd) {
                case 0: stateCurr = STATE_OFF; break;
                case 1: stateCurr = STATE_ERROR; break;
                case 2: stateCurr = statePrev; break;
                case 3: stateCurr = STATE_RED; break;
                case 4:
                    if(stateCurr == STATE_BLINKING || stateCurr == STATE_ERROR) {
                        stateCurr = STATE_ERROR;
                    } else {
                        stateCurr = nextState(stateCurr);
                    }
                    break;
                case 5: stateCurr = STATE_GREEN; break;
                default: stateCurr = STATE_RED;
            }

            statePrev = stateCurr;
            displayLight(stateCurr, controller_id);

            bus_ack.write(true); // Potwierdzenie odebrania danych w tym cyklu
        }
        else if (bus_req.read() == false) {
            bus_ack.write(false); // Reset potwierdzenia, gdy master zwolni żądanie
        }
    }
};

// ============================================================
// Bus Module: BCA Broadcast Bus
// Zapewnia cykliczną dystrybucję danych do wszystkich kontrolerów
// ============================================================
SC_MODULE(BcaBus) {
    sc_in<bool> clk;

    // Porty Mastera
    sc_in<bool>  m_req;
    sc_in<int>   m_cmd;
    sc_out<bool> m_ack;

    // Porty Slave'ów (rozesłanie do 5 urządzeń)
    sc_out<bool> s_req;
    sc_out<int>  s_cmd;
    sc_in<bool>  s_ack[5];

    SC_CTOR(BcaBus) {
        SC_METHOD(bus_logic);
        sensitive << clk.pos();
        dont_initialize();
    }

    void bus_logic() {
    	// Przekazanie danych kontrolera dalej.
    	s_req.write(m_req.read());
    	s_cmd.write(m_cmd.read());

    	// all_ack dla potwierdzonego cyklu 2.
    	bool all_ack = true;
    	// any_ack dla przejścia z 4. do 1.
    	bool any_ack = false;

    	// check the above
    	for (int i = 0; i < 5; i++) {
    		if (s_ack[i].read() == false) all_ack = false;
    	    if (s_ack[i].read() == true)  any_ack = true;
    	}

    	// jeśli master przekazał ostatnio wiadomość
    	if (m_req.read() == true) {
    		// jeśli wszystkie odebrały, potwierdź
    	    m_ack.write(all_ack);
    	} else {
    		// jeśli wszystkie są gotowe, potwierdź
    		m_ack.write(any_ack);
    	}
    }
};

// ============================================================
// Master: System Controller ("Tone Setter")
// ============================================================
SC_MODULE(SystemController) {
    sc_in<bool> clk;

    // Interfejs do magistrali BCA
    sc_out<bool> bus_req;
    sc_out<int>  bus_cmd;
    sc_in<bool>  bus_ack;

    SC_CTOR(SystemController) {
        SC_THREAD(control);
        sensitive << clk.pos();
    }

    // Funkcja blokująca, symulująca transakcję cykl po cyklu
    void send_cmd(int cmd) {
        bus_cmd.write(cmd);
        bus_req.write(true); // Faza 1: Wystawienie żądania i danych na magistralę

        do { wait(); } while (bus_ack.read() == false); // Faza 2: Czekanie na sprzętowe potwierdzenie z magistrali

        bus_req.write(false); // Faza 3: Zakończenie transakcji

        do { wait(); } while (bus_ack.read() == true); // Faza 4: Czekanie na całkowite zwolnienie linii przez Slave'y
    }

    void control() {
        // Inicjalizacja stanu zerowego magistrali
        bus_req.write(false);
        bus_cmd.write(3);
        wait(2); // Czekamy 2 cykle na stabilizację

        std::cout << "=== START: Wymuszenie stanu RED na magistrali ===" << std::endl;
        send_cmd(3); // Start RED

        std::cout << "\n=== TEST 1: Normal FSM cycle on all controllers ===" << std::endl;
        send_cmd(4);
        wait(2); // opóźnienie miedzy transakcjami reprezentowane w cyklach

        std::cout << "\n=== TEST 2: Blink all controllers ===" << std::endl;
        send_cmd(5); // Przejdź najpierw w GREEN
        wait(1);
        // Uwaga: w logice FSM brak bezpośredniego wejścia w BLINK z dowolnego stanu poleceniem.
        // Jeśli komenda 4 na ERROR/BLINK robi trap, możemy tu wymusić ERROR
        send_cmd(1);
        wait(2);

        std::cout << "\n=== TEST 3: Emergency override (VEHIC_X) ===" << std::endl;
        send_cmd(3);
        wait(2);

        std::cout << "\n=== TEST 4: Cykl FSM ===" << std::endl;
        send_cmd(4);
        wait(1);
        send_cmd(4);

        std::cout << "\n>>> SYSTEM CONTROLLER: Relinquishing control\n" << std::endl;
        sc_stop(); // Zakończenie symulacji w sposób elegancki
    }
};

// ============================================================
// Top-Level: Testbench
// ============================================================
SC_MODULE(Testbench) {
    // Deklaracja sygnałów zegara i wewnętrznych
    sc_clock clk;

    // Sygnały Master <-> Magistrala
    sc_signal<bool> m_req;
    sc_signal<int>  m_cmd;
    sc_signal<bool> m_ack;

    // Sygnały Magistrala <-> Slaves
    sc_signal<bool> s_req;
    sc_signal<int>  s_cmd;
    sc_signal<bool> s_ack[5];

    // Moduły
    SystemController* sys_ctrl;
    BcaBus* bus;
    TrafficLightController* controller[5];

    SC_CTOR(Testbench) : clk("clk", 10, SC_NS) { // Zegar 10ns
        // 1. Inicjalizacja Mastera
        sys_ctrl = new SystemController("SystemController");
        sys_ctrl->clk(clk);
        sys_ctrl->bus_req(m_req);
        sys_ctrl->bus_cmd(m_cmd);
        sys_ctrl->bus_ack(m_ack);

        // 2. Inicjalizacja Magistrali BCA
        bus = new BcaBus("BCABroadcastBus");
        bus->clk(clk);
        bus->m_req(m_req);
        bus->m_cmd(m_cmd);
        bus->m_ack(m_ack);
        bus->s_req(s_req);
        bus->s_cmd(s_cmd);

        // 3. Inicjalizacja Slave'ów (Sygnalizacji) i podpinanie ich do magistrali
        for(int i = 0; i < 5; i++) {
            std::string ctrl_name = "Controller_" + std::to_string(i);
            controller[i] = new TrafficLightController(ctrl_name.c_str(), i);
            controller[i]->clk(clk);
            controller[i]->bus_req(s_req);
            controller[i]->bus_cmd(s_cmd);
            controller[i]->bus_ack(s_ack[i]); // Każdy ma swój kanał ACK

            // Rejestracja w magistrali
            bus->s_ack[i](s_ack[i]);
        }
    }

    ~Testbench() {
        delete sys_ctrl;
        delete bus;
        for(int i = 0; i < 5; i++) delete controller[i];
    }
};

// ============================================================
// Main Simulation
// ============================================================
int sc_main(int argc, char* argv[]) {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "ECLIPSE PROJECT: Traffic Light System - Bus Cycle Accurate Model" << std::endl;
    std::cout << "Architecture: SystemController (Master) -> BcaBus -> 5x TrafficLightController (Slaves)" << std::endl;
    std::cout << std::string(80, '=') << "\n" << std::endl;

    Testbench tb("TopLevelTestbench");

    sc_start(); // Testbench automatycznie przerwie symulację przez sc_stop() w kontrolerze

    return 0;
}
