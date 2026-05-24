#include <systemc>
#include <iostream>

using namespace sc_core;


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
}
