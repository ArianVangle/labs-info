#pragma once
#include "generator.hpp"
#include "lazy_sequence.hpp"

template<class T>
RecursiveGenerator<T>::RecursiveGenerator(LazySequence<T>* seq, T (*rule)(Sequence<T>*), size_t maxSize)
    : owner(seq), generationRule(rule), position(0) {
    cache = new CircularBuffer<T>(maxSize);
}

template<class T>
RecursiveGenerator<T>::~RecursiveGenerator() {
    delete cache;
}

template<class T>
T RecursiveGenerator<T>::GetNext() {
    if (!HasNext()) throw InvalidOperationException("No next element");
    T value = generationRule(owner->materialized);
    owner->materialized->Append(value);
    position++;
    return value;
}

template<class T>
bool RecursiveGenerator<T>::HasNext() const {
    return owner->CanGenerate(position);
}

template<class T>
void RecursiveGenerator<T>::Reset() {
    position = 0;
    cache->Clear();
    delete owner->materialized;
    owner->materialized = new MutableArraySequence<T>();
}

template<class T>
T RecursiveGenerator<T>::Current() const {
    if (owner->materialized->GetLength() == 0) {
        throw InvalidOperationException("No current element");
    }
    return owner->materialized->Get(owner->materialized->GetLength() - 1);
}

template<class T>
bool RecursiveGenerator<T>::MoveNext() {
    if (!HasNext()) return false;
    GetNext();
    return true;
}