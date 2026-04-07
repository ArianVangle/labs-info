#pragma once
#include "option.h"

template <class T>
Option<T>::Option() : hasValue(false), value(T()) {
}

template <class T>
Option<T> Option<T>::Some(T val) {
    Option<T> opt;
    opt.hasValue = true;
    opt.value = val;
    return opt;
}

template <class T>
Option<T> Option<T>::None() {
    Option<T> opt;
    opt.hasValue = false;
    return opt;
}

template <class T>
bool Option<T>::IsSome() const {
    return hasValue;
}

template <class T>
bool Option<T>::IsNone() const {
    return !hasValue;
}

template <class T>
T Option<T>::GetValue() const {
    if (!hasValue) 
        throw InvalidOperationException("Option has no value");
    return value;
}