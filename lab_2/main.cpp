#include <iostream>
#include <locale>

#include "ui.h"

int main() {
    try {
        std::locale::global(std::locale("ru_RU.UTF-8"));
    } catch (...) {
        std::locale::global(std::locale(""));
    }

    ConsoleUI ui;
    ui.ShowWelcome();
    ui.ShowMenu();

    return 0;
}