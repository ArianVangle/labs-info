#pragma once
#include "array_sequence.h"


template <class T, class Derived>
void ArraySequence<T, Derived>::AppendInternal(const T& item) {
    items->Resize(items->GetSize() + 1);
    items->Set(items->GetSize() - 1, item);
}

template <class T, class Derived>
void ArraySequence<T, Derived>::PrependInternal(const T& item) {
    DynamicArray<T>* newItems = new DynamicArray<T>(items->GetSize() + 1);
    newItems->Set(0, item);
    for (int i = 0; i < items->GetSize(); i++) {
        newItems->Set(i + 1, items->Get(i));
    }
    delete items;
    items = newItems;
}

template <class T, class Derived>
void ArraySequence<T, Derived>::InsertAtInternal(const T& item, int index) {
    if (index < 0 || index > items->GetSize())
        throw IndexOutOfRangeException("Index out of range");

    DynamicArray<T>* newItems = new DynamicArray<T>(items->GetSize() + 1);
    for (int i = 0; i < index; i++)
        newItems->Set(i, items->Get(i));

    newItems->Set(index, item);
    for (int i = index; i < items->GetSize(); i++)
        newItems->Set(i + 1, items->Get(i));

    delete items;
    items = newItems;
}

template <class T, class Derived>
void ArraySequence<T, Derived>::SetInternal(size_t index, const T& value) {
    items->Set(index, value);
}

template <class T, class Derived>
ArraySequence<T, Derived>::ArraySequence() {
    items = new DynamicArray<T>(0);
}

template <class T, class Derived>
ArraySequence<T, Derived>::ArraySequence(T* itemsArr, int count) {
    items = new DynamicArray<T>(itemsArr, count);
}

template <class T, class Derived>
ArraySequence<T, Derived>::ArraySequence(const LinkedList<T>& list) {
    items = new DynamicArray<T>(list.GetLength());
    for (int i = 0; i < list.GetLength(); i++) {
        items->Set(i, list.Get(i));
    }
}

template <class T, class Derived>
ArraySequence<T, Derived>::ArraySequence(const ArraySequence& other) {
    items = new DynamicArray<T>(*other.items);
}

template <class T, class Derived>
ArraySequence<T, Derived>::~ArraySequence() {
    delete items;
}

template <class T, class Derived>
T ArraySequence<T, Derived>::Get(size_t index) const {
    return items->Get(index);
}

template <class T, class Derived>
size_t ArraySequence<T, Derived>::GetCount() const {
    return items->GetSize();
}

template <class T, class Derived>
int ArraySequence<T, Derived>::GetLength() const {
    return items->GetSize();
}

template <class T, class Derived>
T ArraySequence<T, Derived>::GetFirst() const {
    if (items->GetSize() == 0) throw IndexOutOfRangeException("Sequence is empty");
    return items->Get(0);
}

template <class T, class Derived>
T ArraySequence<T, Derived>::GetLast() const {
    if (items->GetSize() == 0) throw IndexOutOfRangeException("Sequence is empty");
    return items->Get(items->GetSize() - 1);
}

template <class T, class Derived>
IEnumerator<T>* ArraySequence<T, Derived>::GetEnumerator() const {
    return new ArrayEnumerator<T, Derived>(this);
}

template <class T, class Derived>
Sequence<T>* ArraySequence<T, Derived>::Clone() const {
    return new Derived(static_cast<const Derived&>(*this));
}

template <class T, class Derived>
Sequence<T>* ArraySequence<T, Derived>::GetSubsequence(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= items->GetSize() || startIndex > endIndex)
        throw IndexOutOfRangeException("Invalid indices");
    
    auto* res = new MutableArraySequence<T>();
    IEnumerator<T>* en = this->GetEnumerator();
    int idx = 0;
    while (en->MoveNext()) {
        if (idx >= startIndex && idx <= endIndex) {
            res->Append(en->Current());
        }
        idx++;
    }
    delete en;
    return res;
}

template <class T, class Derived>
Sequence<T>* ArraySequence<T, Derived>::Concat(const Sequence<T>& list) {
    auto* res = new MutableArraySequence<T>();
    
    IEnumerator<T>* enThis = this->GetEnumerator();
    while (enThis->MoveNext()) res->Append(enThis->Current());
    delete enThis;

    IEnumerator<T>* enList = list.GetEnumerator();
    while (enList->MoveNext()) res->Append(enList->Current());
    delete enList;

    return res;
}

template <class T, class Derived>
template <class R>
Sequence<R>* ArraySequence<T, Derived>::Map(std::function<R(T)> func) const {
    auto* res = new MutableArraySequence<R>();
    IEnumerator<T>* en = this->GetEnumerator();
    while (en->MoveNext()) {
        res->Append(func(en->Current()));
    }
    delete en;
    return res;
}

template <class T, class Derived>
Sequence<T>* ArraySequence<T, Derived>::Where(std::function<bool(T)> func) const {
    auto* res = new MutableArraySequence<T>();
    IEnumerator<T>* en = this->GetEnumerator();
    while (en->MoveNext()) {
        if (func(en->Current())) {
            res->Append(en->Current());
        }
    }
    delete en;
    return res;
}

template <class T, class Derived>
T ArraySequence<T, Derived>::Reduce(std::function<T(T, T)> func, T start) const {
    T result = start;
    IEnumerator<T>* en = this->GetEnumerator();
    while (en->MoveNext()) {
        result = func(result, en->Current());
    }
    delete en;
    return result;
}

template <class T, class Derived>
Option<T> ArraySequence<T, Derived>::TryGet(int index) const {
    try {
        return Option<T>::Some(Get(index));
    } catch (...) {
        return Option<T>::None();
    }
}

template <class T, class Derived>
T ArraySequence<T, Derived>::operator[](int index) const {
    return Get(index);
}


template <class T>
MutableArraySequence<T>::MutableArraySequence() : ArraySequence<T, MutableArraySequence<T>>() {}

template <class T>
MutableArraySequence<T>::MutableArraySequence(T* items, int count) : ArraySequence<T, MutableArraySequence<T>>(items, count) {}

template <class T>
MutableArraySequence<T>::MutableArraySequence(const MutableArraySequence<T>& other) : ArraySequence<T, MutableArraySequence<T>>(other) {}

template <class T>
Sequence<T>* MutableArraySequence<T>::Clone() const {
    return new MutableArraySequence<T>(*this);
}

template <class T>
Sequence<T>* MutableArraySequence<T>::Append(const T& item) {
    this->AppendInternal(item);
    return this;
}

template <class T>
Sequence<T>* MutableArraySequence<T>::Prepend(const T& item) {
    this->PrependInternal(item);
    return this;
}

template <class T>
Sequence<T>* MutableArraySequence<T>::InsertAt(const T& item, int index) {
    this->InsertAtInternal(item, index);
    return this;
}

template <class T>
void MutableArraySequence<T>::Set(size_t index, const T& value) {
    this->SetInternal(index, value);
}


template <class T>
ImmutableArraySequence<T>::ImmutableArraySequence() : ArraySequence<T, ImmutableArraySequence<T>>() {}

template <class T>
ImmutableArraySequence<T>::ImmutableArraySequence(T* items, int count) : ArraySequence<T, ImmutableArraySequence<T>>(items, count) {}

template <class T>
ImmutableArraySequence<T>::ImmutableArraySequence(const ImmutableArraySequence<T>& other) : ArraySequence<T, ImmutableArraySequence<T>>(other) {}

template <class T>
Sequence<T>* ImmutableArraySequence<T>::Clone() const {
    return new ImmutableArraySequence<T>(*this);
}

template <class T>
Sequence<T>* ImmutableArraySequence<T>::Append(const T& item) {
    auto* clone = static_cast<ImmutableArraySequence<T>*>(this->Clone());
    clone->AppendInternal(item);
    return clone;
}

template <class T>
Sequence<T>* ImmutableArraySequence<T>::Prepend(const T& item) {
    auto* clone = static_cast<ImmutableArraySequence<T>*>(this->Clone());
    clone->PrependInternal(item);
    return clone;
}

template <class T>
Sequence<T>* ImmutableArraySequence<T>::InsertAt(const T& item, int index) {
    auto* clone = static_cast<ImmutableArraySequence<T>*>(this->Clone());
    clone->InsertAtInternal(item, index);
    return clone;
}

template <class T>
void ImmutableArraySequence<T>::Set(size_t, const T&) {
    throw InvalidOperationException("Cannot modify immutable sequence");
}