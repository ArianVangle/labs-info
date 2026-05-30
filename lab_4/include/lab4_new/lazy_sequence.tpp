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
    : generator(nullptr), materialized(nullptr), cardinalLength(0), isOwner(true), nextInChain(nullptr) {
    materialized = new MutableArraySequence<T>();
}

template<class T>
LazySequence<T>::LazySequence(T* items, size_t count) 
    : generator(nullptr), materialized(nullptr), cardinalLength(count), isOwner(true), nextInChain(nullptr) {
    materialized = new MutableArraySequence<T>(items, count);
}

template<class T>
LazySequence<T>::LazySequence(Sequence<T>* seq) 
    : generator(nullptr), materialized(nullptr), cardinalLength(seq->GetLength()), isOwner(true), nextInChain(nullptr) {
    materialized = new MutableArraySequence<T>();
    for (size_t i = 0; i < seq->GetLength(); i++) {
        materialized->Append(seq->Get(i));
    }
}

template<class T>
LazySequence<T>::LazySequence(T (*rule)(Sequence<T>*), Cardinal len)
    : materialized(nullptr), cardinalLength(len), isOwner(true), nextInChain(nullptr) {
    materialized = new MutableArraySequence<T>();
    generator = new RecursiveGenerator<T>(this, rule);
}

template<class T>
LazySequence<T>::LazySequence(const LazySequence<T>& other) 
    : generator(nullptr), materialized(nullptr), cardinalLength(other.cardinalLength), isOwner(true), nextInChain(nullptr) {
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
T LazySequence<T>::Get(const OrdinalIndex& idx) const {
    if (idx.block == 0) {
        return Get(idx.offset);
    }

    if (nextInChain != nullptr) {
        const LazySequence<T>* nextLazy = dynamic_cast<const LazySequence<T>*>(nextInChain);
        
        if (nextLazy) {
            return nextLazy->Get({idx.block - 1, idx.offset});
        } else {
            if (idx.block == 1) {
                return nextInChain->Get(idx.offset);
            }
            throw IndexOutOfRangeException("Block index out of range in concatenation chain");
        }
    }
    
    throw IndexOutOfRangeException("Block index out of range");
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
    if (cardinalLength.IsInfinite()) {
        throw InvalidOperationException("Cannot append to infinite sequence");
    }
    return InsertAt(item, cardinalLength.GetValue());
}

template<class T>
Sequence<T>* LazySequence<T>::Prepend(const T& item) {
    return InsertAt(item, 0);
}

template<class T>
Sequence<T>* LazySequence<T>::InsertAt(const T& item, int index) {
    if (index < 0 || (cardinalLength.IsFinite() && index > (int)cardinalLength.GetValue()))
        throw IndexOutOfRangeException("Index out of range");

    auto* result = new LazySequence<T>();
    auto* itemSeq = new MutableArraySequence<T>();
    itemSeq->Append(item);

    result->generator = new InsertAtGenerator<T>(this->generator, itemSeq, index);
    
    result->cardinalLength = cardinalLength.IsFinite() 
        ? Cardinal(cardinalLength.GetValue() + 1) 
        : Cardinal::Infinity();
        
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
    
    bool isOtherInfinite = false;
    if (const auto* lazyOther = dynamic_cast<const LazySequence<T>*>(&other)) {
        isOtherInfinite = lazyOther->GetCardinalLength().IsInfinite();
    } else if (other.GetLength() == -1) {
        isOtherInfinite = true;
    }

    if (cardinalLength.IsInfinite() || isOtherInfinite) {
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
    auto* result = new LazySequence<R>();
    result->generator = new MapGenerator<T, R>(this->generator, func);
    result->cardinalLength = cardinalLength;
    result->isOwner = true;
    return result;
}

template<class T>
Sequence<T>* LazySequence<T>::Where(std::function<bool(T)> func) const {
    auto* result = new LazySequence<T>();
    result->generator = new WhereGenerator<T>(this->generator, func);
    result->cardinalLength = cardinalLength;
    result->isOwner = true;
    return result;
}

template<class T>
void LazySequence<T>::MaterializeUpTo(size_t index) const {
    if (generator == nullptr) return;
    while ((int)materialized->GetLength() <= (int)index && CanGenerate(materialized->GetLength())) {
        T value = generator->GetNext();
        if (!generator->IsSelfMaterializing()) {
            materialized->Append(value);
        }
    }
}

template<class T>
Sequence<T>* LazySequence<T>::Concat(const Sequence<T>& list) {
    auto* result = new LazySequence<T>();
    result->nextInChain = &list;
    result->isOwner = true;
    
    if (cardinalLength.IsInfinite()) {
        result->cardinalLength = Cardinal::Infinity();
    } else if (list.GetLength() == -1) {
        result->cardinalLength = Cardinal::Infinity();
    } else {
        result->cardinalLength = cardinalLength + Cardinal(list.GetLength());
    }
    
    result->generator = new ConcatGenerator<T>(this, &list);
    return result;
}



template<class T>
T LazySequence<T>::Reduce(std::function<T(T, T)> func, T start) const {
    int len = GetLength();
    if (len == -1) throw InvalidOperationException("Cannot reduce infinite sequence");
    
    T result = start;
    for (int i = 0; i < len; i++) {
        result = func(result, Get(i));
    }
    return result;
}
template<class T>
Sequence<T>* LazySequence<T>::GetMaterializedSequence() const {
    return materialized->Clone();
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