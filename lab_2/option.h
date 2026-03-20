#pragma once
#include "exceptions.h"

template <class T>
class Option {
   private:
    bool hasValue;
    T value;

   public:
    Option() : hasValue(false), value(T()) {}

    static Option<T> Some(T val) {
        Option<T> opt;
        opt.hasValue = true;
        opt.value = val;
        return opt;
    }

    static Option<T> None() {
        Option<T> opt;
        opt.hasValue = false;
        return opt;
    }

    bool IsSome() const { return hasValue; }

    bool IsNone() const { return !hasValue; }

    T GetValue() const {
        if (!hasValue) throw InvalidOperationException("Option has no value");
        return value;
    }
};