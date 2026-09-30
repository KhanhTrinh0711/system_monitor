//
// Created by trinh on 8/6/26.
//

#include "../src/cpu.h"
#include "../src/memory.h"
#include "../src/process.h"

#include <iostream>
#include <string>

void print_menu() {
    std::cout << "========== SYSTEM MONITOR ==========\n"
              << "1. CPU information\n"
              << "2. Memory information\n"
              << "3. Process list\n"
              << "q. Quit\n"
              << "Choose an option: ";
}

void clear_output() {
    std::cout << "\033[2J\033[H" << std::flush;
}

int main() {
    std::string choice;

    while (true) {
        print_menu();
        if (!std::getline(std::cin, choice)) {
            break;
        }

        bool mode_selected = false;
        if (choice == "1") {
            clear_output();
            cpu();
            mode_selected = true;
        } else if (choice == "2") {
            clear_output();
            MEMINFO mem_info;
            memory(mem_info);
            mode_selected = true;
        } else if (choice == "3") {
            clear_output();
            process();
            mode_selected = true;
        } else if (choice == "q") {
            break;
        } else {
            std::cout << "Invalid choice. Press Enter to continue...";
            std::getline(std::cin, choice);
        }

        if (mode_selected) {
            std::cout << "\nPress Enter to return to the menu...";
            std::getline(std::cin, choice);
            clear_output();
        }
    }

    return 0;
}
