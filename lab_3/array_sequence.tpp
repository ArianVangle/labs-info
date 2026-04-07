#pragma once
#include "array_sequence.h"

template <class T>
Sequence<T>* ArraySequence<T>::AppendInternal(const T& item) {
    items->Resize(items->GetSize() + 1);
    items->Set(items->GetSize() - 1, item);
    return this;
}

template <class T>
Sequence<T>* ArraySequence<T>::PrependInternal(const T& item) {
    DynamicArray<T>* newItems = new DynamicArray<T>(items->GetSize() + 1);
    newItems->Set(0, item);
    for (int i = 0; i < items->GetSize(); i++) {
        newItems->Set(i + 1, items->Get(i));
    }
    delete items;
    items = newItems;
    return this;
}

template <class T>
Sequence<T>* ArraySequence<T>::InsertAtInternal(const T& item, int index) {
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
    return this;
}

template <class T>
void ArraySequence<T>::SetInternal(size_t index, const T& value) {
    items->Set(index, value);
}

template <class T>
ArraySequence<T>::ArraySequence() {
    items = new DynamicArray<T>(0);
}

template <class T>
ArraySequence<T>::ArraySequence(T* itemsArr, int count) {
    items = new DynamicArray<T>(itemsArr, count);
}

template <class T>
ArraySequence<T>::ArraySequence(const LinkedList<T>& list) {
    items = new DynamicArray<T>(list.GetLength());
    for (int i = 0; i < list.GetLength(); i++) {
        items->Set(i, list.Get(i));
    }
}

template <class T>
ArraySequence<T>::ArraySequence(const ArraySequence<T>& other) {
    items = new DynamicArray<T>(*other.items);
}

template <class T>
ArraySequence<T>::~ArraySequence() {
    delete items;
}

template <class T>
T ArraySequence<T>::Get(size_t index) const {
    return items->Get(index);
}

template <class T>
size_t ArraySequence<T>::GetCount() const {
    return items->GetSize();
}

template <class T>
Sequence<T>* ArraySequence<T>::Clone() const {
    ArraySequence<T>* clone = CreateEmpty();
    for (int i = 0; i < items->GetSize(); i++) {
        clone->items->Set(i, items->Get(i));
    }
    return clone;
}

template <class T>
T ArraySequence<T>::GetFirst() const {
    if (items->GetSize() == 0) 
        throw IndexOutOfRangeException("Sequence is empty");
    return items->Get(0);
}

template <class T>
T ArraySequence<T>::GetLast() const {
    if (items->GetSize() == 0) 
        throw IndexOutOfRangeException("Sequence is empty");
    return items->Get(items->GetSize() - 1);
}

template <class T>
Sequence<T>* ArraySequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= items->GetSize() || startIndex > endIndex)
        throw IndexOutOfRangeException("Invalid indices");

    int len = endIndex - startIndex + 1;
    ArraySequence<T>* res = CreateEmpty();
    res->items->Resize(len);

    for (int i = 0; i < len; i++) {
        res->items->Set(i, items->Get(startIndex + i));
    }
    return res;
}

template <class T>
int ArraySequence<T>::GetLength() const {
    return items->GetSize();
}

template <class T>
Sequence<T>* ArraySequence<T>::Append(const T& item) {
    return ((ArraySequence<T>*)Instance())->AppendInternal(item);
}

template <class T>
Sequence<T>* ArraySequence<T>::Prepend(const T& item) {
    return ((ArraySequence<T>*)Instance())->PrependInternal(item);
}

template <class T>
Sequence<T>* ArraySequence<T>::InsertAt(const T& item, int index) {
    return ((ArraySequence<T>*)Instance())->InsertAtInternal(item, index);
}

template <class T>
void ArraySequence<T>::Set(size_t index, const T& value) {
    ((ArraySequence<T>*)Instance())->SetInternal(index, value);
}

template <class T>
Sequence<T>* ArraySequence<T>::Concat(Sequence<T>* list) {
    int currentLen = items->GetSize();
    int addLen = list->GetLength();
    items->Resize(currentLen + addLen);
    IEnumerator<T>* en = list->GetEnumerator();
    int i = currentLen;
    while (en->MoveNext()) {
        items->Set(i++, en->Current());
    }
    delete en;
    return this;
}

template <class T>
template <class R>
Sequence<R>* ArraySequence<T>::Map(std::function<R(T)> func) const {
    R* newArr = new R[items->GetSize()];
    for (int i = 0; i < items->GetSize(); i++) {
        newArr[i] = func(items->Get(i));
    }
    Sequence<R>* res = new MutableArraySequence<R>(newArr, items->GetSize());
    delete[] newArr;
    return res;
}

template <class T>
Sequence<T>* ArraySequence<T>::Where(std::function<bool(T)> func) const {
    int count = 0;
    for (int i = 0; i < items->GetSize(); i++) {
        if (func(items->Get(i))) count++;
    }

    T* newArr = new T[count];
    int idx = 0;
    
    for (int i = 0; i < items->GetSize(); i++) {
        if (func(items->Get(i))) newArr[idx++] = items->Get(i);
    }

    ArraySequence<T>* res = CreateEmpty();
    res->items->Resize(count);

    for (int i = 0; i < count; i++) {
        res->items->Set(i, newArr[i]);
    }
    
    delete[] newArr;
    return res;
}

template <class T>
T ArraySequence<T>::Reduce(std::function<T(T, T)> func, T start) const {
    T result = start;
    IEnumerator<T>* en = this->GetEnumerator();
    while (en->MoveNext()) {
        result = func(result, en->Current());
    }
    delete en;
    return result;
}

template <class T>
Option<T> ArraySequence<T>::TryGet(int index) const {
    try {
        return Option<T>::Some(Get(index));
    } catch (...) {
        return Option<T>::None();
    }
}

template <class T>
IEnumerator<T>* ArraySequence<T>::GetEnumerator() const {
    return new ArrayEnumerator<T>(this);
}

template <class T>
T ArraySequence<T>::operator[](int index) const {
    return Get(index);
}

template <class T>
MutableArraySequence<T>::MutableArraySequence() : ArraySequence<T>() {}

template <class T>
MutableArraySequence<T>::MutableArraySequence(T* items, int count) : ArraySequence<T>(items, count) {}

template <class T>
MutableArraySequence<T>::MutableArraySequence(const MutableArraySequence<T>& other) : ArraySequence<T>(other) {}

template <class T>
Sequence<T>* MutableArraySequence<T>::Clone() const {
    return new MutableArraySequence<T>(*this);
}

template <class T>
Sequence<T>* MutableArraySequence<T>::Instance() {
    return this;
}

template <class T>
ArraySequence<T>* MutableArraySequence<T>::CreateEmpty() const {
    return new MutableArraySequence<T>();
}

template <class T>
ImmutableArraySequence<T>::ImmutableArraySequence() : ArraySequence<T>() {}

template <class T>
ImmutableArraySequence<T>::ImmutableArraySequence(T* items, int count) : ArraySequence<T>(items, count) {}

template <class T>
ImmutableArraySequence<T>::ImmutableArraySequence(const ImmutableArraySequence<T>& other) : ArraySequence<T>(other) {}

template <class T>
Sequence<T>* ImmutableArraySequence<T>::Clone() const {
    return new ImmutableArraySequence<T>(*this);
}

template <class T>
Sequence<T>* ImmutableArraySequence<T>::Instance() {
    return this->Clone();
}

template <class T>
ArraySequence<T>* ImmutableArraySequence<T>::CreateEmpty() const {
    return new ImmutableArraySequence<T>();
}