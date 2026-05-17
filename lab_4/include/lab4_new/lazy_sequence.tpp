#pragma once

#include "lazy_sequence.hpp"
#include "../lab3_base/array_sequence.h"
#include "generator.hpp"


template<class T>
bool LazySequence<T>::CanGenerate(size_t position) const {
    return cardinalLength.IsInfinite() || position < cardinalLength.GetValue();
}

template<class T>
LazySequence<T>::LazySequence() 
    : generator(nullptr), materialized(nullptr), cardinalLength(0), isOwner(true) {
    materialized = new MutableArraySequence<T>();
}

template<class T>
LazySequence<T>::LazySequence(T* items, size_t count) 
    : generator(nullptr), materialized(nullptr), cardinalLength(count), isOwner(true) {
    materialized = new MutableArraySequence<T>(items, count);
}

template<class T>
LazySequence<T>::LazySequence(Sequence<T>* seq) 
    : generator(nullptr), materialized(nullptr), cardinalLength(seq->GetLength()), isOwner(true) {
    materialized = new MutableArraySequence<T>();
    for (size_t i = 0; i < seq->GetLength(); i++) {
        materialized->Append(seq->Get(i));
    }
}

template<class T>
LazySequence<T>::LazySequence(T (*rule)(Sequence<T>*), Cardinal len)
    : materialized(nullptr), cardinalLength(len), isOwner(true) {
    materialized = new MutableArraySequence<T>();
    generator = new RecursiveGenerator<T>(this, rule);
}

template<class T>
LazySequence<T>::LazySequence(const LazySequence<T>& other) 
    : generator(nullptr), materialized(nullptr), cardinalLength(other.cardinalLength), isOwner(true) {
    materialized = other.materialized->Clone();
}

template<class T>
LazySequence<T>::~LazySequence() {
    if (isOwner && materialized != nullptr) { delete materialized; materialized = nullptr; }
    if (isOwner && generator != nullptr) { delete generator; generator = nullptr; }
}

template<class T>
T LazySequence<T>::GetFirst() const {
    if (materialized->GetLength() == 0 && !CanGenerate(0))
        throw InvalidOperationException("Sequence is empty");
    MaterializeUpTo(0);
    return materialized->Get(0);
}

template<class T>
T LazySequence<T>::GetLast() const {
    if (cardinalLength.IsInfinite())
        throw InvalidOperationException("Cannot get last of infinite sequence");
    if (cardinalLength.GetValue() == 0)
        throw InvalidOperationException("Sequence is empty");
    return Get(cardinalLength.GetValue() - 1);
}

template<class T>
T LazySequence<T>::Get(size_t index) const {
    if (cardinalLength.IsFinite() && index >= cardinalLength.GetValue())
        throw IndexOutOfRangeException("Index out of range");
    MaterializeUpTo(index);
    return materialized->Get(index);
}

template<class T>
LazySequence<T>* LazySequence<T>::GetSubsequence(int start, int end) const {
    if (start < 0 || (cardinalLength.IsFinite() && end >= (int)cardinalLength.GetValue()) || start > end)
        throw IndexOutOfRangeException("Invalid indices");
    MaterializeUpTo(end);
    auto* result = new LazySequence<T>();
    for (int i = start; i <= end; i++) {
        result->materialized->Append(materialized->Get(i));
    }
    result->cardinalLength = Cardinal(end - start + 1);
    return result;
}

template<class T>
int LazySequence<T>::GetLength() const {
    return cardinalLength.IsFinite() ? (int)cardinalLength.GetValue() : -1;
}

template<class T>
Cardinal LazySequence<T>::GetCardinalLength() const { return cardinalLength; }

template<class T>
size_t LazySequence<T>::GetMaterializedCount() const { return materialized->GetLength(); }

template<class T>
Sequence<T>* LazySequence<T>::Append(const T& item) {
    auto* result = new LazySequence<T>(*this);
    result->MaterializeUpTo(result->materialized->GetLength());
    result->materialized->Append(item);
    if (result->cardinalLength.IsFinite()) {
        result->cardinalLength = Cardinal(result->cardinalLength.GetValue() + 1);
    }
    return result;
}

template<class T>
Sequence<T>* LazySequence<T>::Prepend(const T& item) {
    auto* result = new LazySequence<T>();
    result->materialized->Append(item);
    for (size_t i = 0; i < (size_t)materialized->GetLength(); i++) {
        result->materialized->Append(materialized->Get(i));
    }
    result->cardinalLength = cardinalLength.IsFinite() ? Cardinal(cardinalLength.GetValue() + 1) : Cardinal::Infinity();
    return result;
}

template<class T>
Sequence<T>* LazySequence<T>::InsertAt(const T& item, int index) {
    if (index < 0 || (cardinalLength.IsFinite() && index > (int)cardinalLength.GetValue()))
        throw IndexOutOfRangeException("Index out of range");
    
    MaterializeUpTo(index > 0 ? index - 1 : 0);
    auto* result = new LazySequence<T>();
    for (int i = 0; i < index; i++) 
        result->materialized->Append(materialized->Get(i));

    result->materialized->Append(item);
    for (size_t i = index; i < (size_t)materialized->GetLength(); i++) 
        result->materialized->Append(materialized->Get(i));

    result->cardinalLength = cardinalLength.IsFinite() ? Cardinal(cardinalLength.GetValue() + 1) : Cardinal::Infinity();
    return result;
}

template<class T>
Sequence<T>* LazySequence<T>::Concat(const Sequence<T>& other) {
    auto* result = new LazySequence<T>();
    result->materialized = new MutableArraySequence<T>();
    
    if (cardinalLength.IsInfinite()) {
        result->cardinalLength = Cardinal::Infinity();
    } else {
        result->cardinalLength = cardinalLength + Cardinal(other.GetLength());
    }
    
    result->generator = new ConcatGenerator<T>(this->generator, &other);
    result->isOwner = true;
    return result;
}

template<class T>
Sequence<T>* LazySequence<T>::InsertSequenceAt(const Sequence<T>& other, int index) {
    if (index < 0 || (cardinalLength.IsFinite() && index > (int)cardinalLength.GetValue())) {
        throw IndexOutOfRangeException("Insert index out of range");
    }

    auto* result = new LazySequence<T>();
    result->materialized = new MutableArraySequence<T>();
    
    if (cardinalLength.IsInfinite() || other.GetCardinalLength().IsInfinite()) {
        result->cardinalLength = Cardinal::Infinity();
    } else {
        result->cardinalLength = cardinalLength + Cardinal(other.GetLength());
    }

    result->generator = new InsertAtGenerator<T>(this->generator, &other, index);
    result->isOwner = true;
    return result;
}


template<class T>
template<class R>
Sequence<R>* LazySequence<T>::Map(std::function<R(T)> func) const {
    auto* result = new MutableArraySequence<R>();
    for (size_t i = 0; i < (size_t)materialized->GetLength(); i++) {
        result->Append(func(materialized->Get(i)));
    }
    return result;
}

template<class T>
Sequence<T>* LazySequence<T>::Where(std::function<bool(T)> func) const {
    auto* result = new MutableArraySequence<T>();
    for (size_t i = 0; i < (size_t)materialized->GetLength(); i++) {
        T val = materialized->Get(i);
        if (func(val)) result->Append(val);
    }
    return result;
}

template<class T>
T LazySequence<T>::Reduce(std::function<T(T, T)> func, T start) const {
    T result = start;
    for (size_t i = 0; i < (size_t)materialized->GetLength(); i++) {
        result = func(result, materialized->Get(i));
    }
    return result;
}

template<class T>
Sequence<T>* LazySequence<T>::GetMaterializedSequence() const {
    return materialized->Clone();
}

template<class T>
void LazySequence<T>::MaterializeUpTo(size_t index) const {
    if (generator == nullptr) return;
    while ((int)materialized->GetLength() <= (int)index && CanGenerate(materialized->GetLength())) {
        T value = generator->GetNext();
        materialized->Append(value);
    }
}

template<class T>
double LazySequence<T>::GetMemoizationRatio() const {
    if (cardinalLength.IsInfinite()) return 0.0;
    return (double)materialized->GetLength() / cardinalLength.GetValue() * 100.0;
}

template<class T>
size_t LazySequence<T>::GetCount() const {
    return cardinalLength.IsFinite() ? cardinalLength.GetValue() : materialized->GetLength();
}

template<class T>
Sequence<T>* LazySequence<T>::Clone() const {
    return new LazySequence<T>(*this);
}

template<class T>
void LazySequence<T>::Set(size_t index, const T& value) {
    if (cardinalLength.IsFinite() && index >= cardinalLength.GetValue())
        throw IndexOutOfRangeException("Index out of range");
    MaterializeUpTo(index);
    Sequence<T>* newMat = new MutableArraySequence<T>();
    for (size_t i = 0; i < (size_t)materialized->GetLength(); i++) {
        newMat->Append(i == index ? value : materialized->Get(i));
    }
    delete materialized;
    materialized = newMat;
}

template<class T>
Option<T> LazySequence<T>::TryGet(int index) const {
    try { return Option<T>::Some(Get(index)); }
    catch (...) { return Option<T>::None(); }
}

template<class T>
IEnumerator<T>* LazySequence<T>::GetEnumerator() const {
    return new RecursiveGenerator<T>(const_cast<LazySequence<T>*>(this), 
        [](Sequence<T>* prev) -> T {
            if (prev->GetLength() == 0) return T();
            return prev->Get(prev->GetLength() - 1);
        });
}
template<class T>
T LazySequence<T>::operator[](int index) const { return Get(index); }