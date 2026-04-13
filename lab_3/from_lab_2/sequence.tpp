#pragma once
#include "sequence.h"

template <class T>
bool Sequence<T>::operator==(const Sequence<T>& other) const {
    if (this->GetLength() != other.GetLength()) 
        return false;
    for (int i = 0; i < this->GetLength(); i++)
        if (!(this->Get(i) == other.Get(i))) 
            return false;
    return true;
}

template <class T>
Sequence<T>* Sequence<T>::operator+(Sequence<T>& other) {
    return this->Concat(other);
}

template <class T>
template <class R>
Sequence<R>* Sequence<T>::Map(std::function<R(T)> func) const {
    return nullptr;
}