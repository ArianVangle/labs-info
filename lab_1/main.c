#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interface.h"
#include "tests.h"

int main(int argc, char** argv) {
    if (argc > 1 && strcmp(argv[1], "--test") == 0) {
        run_all_tests();
        return 0;
    }

    run_interactive_mode();

    return 0;
}