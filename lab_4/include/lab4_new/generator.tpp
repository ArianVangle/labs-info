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
    Sequence<T>* context = owner->GetMaterializedSequence();
    T value = generationRule(context);
    delete context;
    cache->Append(value);
    position++;
    return cache->Get(cache->GetLength() - 1);
}

template<class T>
bool RecursiveGenerator<T>::HasNext() const {
    return owner->CanGenerate(position);
}

template<class T>
void RecursiveGenerator<T>::Reset() {
    position = 0;
    cache->Clear();
}

template<class T>
T RecursiveGenerator<T>::Current() const {
    if (cache->GetLength() == 0) throw InvalidOperationException("No current element");
    return cache->Get(cache->GetLength() - 1);
}

template<class T>
bool RecursiveGenerator<T>::MoveNext() {
    if (!HasNext()) return false;
    GetNext();
    return true;
}