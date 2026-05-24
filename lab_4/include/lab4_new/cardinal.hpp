#pragma once

#include "../lab3_base/exceptions.h"
#include <string>

class Cardinal {
private:
    bool isFinite;
    size_t value;

public:
    Cardinal();
    Cardinal(size_t n);
    
    static Cardinal Infinity();
    
    bool IsFinite() const;
    bool IsInfinite() const;
    size_t GetValue() const;
    
    bool operator==(const Cardinal& other) const;
    bool operator<(const Cardinal& other) const;
    bool operator<=(const Cardinal& other) const;
    
    Cardinal operator+(const Cardinal& other) const;
    Cardinal operator-(const Cardinal& other) const;
    
    std::string ToString() const;
};

struct OrdinalIndex {
    size_t block;
    size_t offset;

    OrdinalIndex(size_t b = 0, size_t o = 0) : block(b), offset(o) {}
    std::string ToString() const {
        return block == 0 ? std::to_string(offset) 
                          : "ω·" + std::to_string(block) + " + " + std::to_string(offset);
    }

    bool operator<(const OrdinalIndex& other) const {
        return (block < other.block) || (block == other.block && offset < other.offset);
    }
};

#include "cardinal.tpp"