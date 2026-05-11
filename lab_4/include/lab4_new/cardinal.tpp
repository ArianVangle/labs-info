#pragma once

#include "cardinal.hpp"

inline Cardinal::Cardinal() : isFinite(true), value(0) {}

inline Cardinal::Cardinal(size_t n) : isFinite(true), value(n) {}

inline Cardinal Cardinal::Infinity() {
    Cardinal c;
    c.isFinite = false;
    return c;
}

inline bool Cardinal::IsFinite() const { return isFinite; }

inline bool Cardinal::IsInfinite() const { return !isFinite; }

inline size_t Cardinal::GetValue() const {
    if (!isFinite) throw InvalidOperationException("Cannot get value of infinity");
    return value;
}

inline bool Cardinal::operator==(const Cardinal& other) const {
    if (isFinite != other.isFinite) return false;
    return !isFinite || value == other.value;
}

inline bool Cardinal::operator<(const Cardinal& other) const {
    if (isFinite && other.IsInfinite()) return true;
    if (IsInfinite() && other.isFinite) return false;
    return isFinite && value < other.value;
}

inline bool Cardinal::operator<=(const Cardinal& other) const {
    return *this == other || *this < other;
}

inline Cardinal Cardinal::operator+(const Cardinal& other) const {
    if (!isFinite || !other.isFinite) return Infinity();
    return Cardinal(value + other.value);
}

inline Cardinal Cardinal::operator-(const Cardinal& other) const {
    if (!isFinite) return Infinity();
    if (!other.isFinite) throw InvalidOperationException("Cannot subtract infinity");
    if (value < other.value) throw IndexOutOfRangeException("Negative cardinal");
    return Cardinal(value - other.value);
}

inline std::string Cardinal::ToString() const {
    return isFinite ? std::to_string(value) : "∞";
}