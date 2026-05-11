#pragma once

#include "generator.hpp"
#include "lazy_sequence.hpp"

template<class T>
Generator<T>::Generator(LazySequence<T>* seq, T (*rule)(Sequence<T>*), size_t maxSize)
    : owner(seq), generationRule(rule), position(0) {
    cache = new CircularBuffer<T>(maxSize);
}

template<class T>
Generator<T>::~Generator() {
    if (cache != nullptr) {
        delete cache;
        cache = nullptr;
    }
}

template<class T>
T Generator<T>::Current() const {
    if (cache->GetLength() == 0) {
        throw InvalidOperationException("No current element");
    }
    return cache->Get(cache->GetLength() - 1);
}

template<class T>
bool Generator<T>::MoveNext() {
    if (!HasNext()) return false;
    GetNext();
    return true;
}

template<class T>
void Generator<T>::Reset() {
    position = 0;
    cache->Clear();
}

template<class T>
T Generator<T>::GetNext() {
    if (!HasNext()) {
        throw InvalidOperationException("No next element");
    }

    if (cache->GetLength() == 0) {
        Sequence<T>* context = owner->GetMaterializedSequence();
        T value = generationRule(context);
        delete context;
        
        cache->Append(value);
    } else {
        Sequence<T>* context = owner->GetMaterializedSequence();
        T value = generationRule(context);
        delete context;
        
        cache->Append(value);
    }

    position++;
    return cache->Get(cache->GetLength() - 1);
}

template<class T>
bool Generator<T>::HasNext() const {
    return owner->CanGenerate(position);
}

template<class T>
Option<T> Generator<T>::TryGetNext() {
    try {
        return Option<T>::Some(GetNext());
    } catch (...) {
        return Option<T>::None();
    }
}

template<class T>
size_t Generator<T>::GetPosition() const { 
    return position; 
}

template<class T>
size_t Generator<T>::GetCacheSize() const { 
    return cache->GetLength(); 
}