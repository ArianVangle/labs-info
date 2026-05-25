#pragma once
#include <iostream>
#include "ring.h"

class RingRenderer {
public:
    static void Render(const Ring& ring, std::ostream& out = std::cout) {
        out << ring.GetColor();
        int width = ring.GetSize() * 2 + 1;
        for (int i = 0; i < width; ++i) out << "█";
        out << "\033[0m";
    }
};