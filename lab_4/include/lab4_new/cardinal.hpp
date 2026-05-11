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

#include "cardinal.tpp"