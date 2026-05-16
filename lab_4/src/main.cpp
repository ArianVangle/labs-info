#include <iostream>
#include <locale>
#include "../include/lab4_new/ui.hpp"

int main() {
    try {
        std::locale::global(std::locale("ru_RU.UTF-8"));
    } catch (...) {
        std::locale::global(std::locale(""));
    }
    
    std::cout << "\n";
    ConsoleUI ui;
    ui.ShowMenu();
    
    return 0;
}