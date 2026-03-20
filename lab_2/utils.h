#pragma once
#include <string>

inline int VisibleLength(const std::string& str) {
    int count = 0;
    for (size_t i = 0; i < str.length(); i++) {
        if ((str[i] & 0b11000000) != 0b10000000) {
            count++;
        }
    }
    return count;
}