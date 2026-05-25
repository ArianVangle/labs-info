#pragma once
#include "list_sequence.h"

template <class T>
Sequence<T>* ListSequence<T>::AppendInternal(const T& item) {
    items->Append(item);
    return this;
}

template <class T>
Sequence<T>* ListSequence<T>::PrependInternal(const T& item) {
    items->Prepend(item);
    return this;
}

template <class T>
Sequence<T>* ListSequence<T>::InsertAtInternal(const T& item, int index) {
    items->InsertAt(item, index);
    return this;
}

template <class T>
void ListSequence<T>::SetInternal(size_t index, const T& value) {
    items->Set(index, value);
}

template <class T>
ListSequence<T>::ListSequence() {
    items = new LinkedList<T>();
}

template <class T>
ListSequence<T>::ListSequence(T* itemsArr, int count) {
    items = new LinkedList<T>(itemsArr, count);
}

template <class T>
ListSequence<T>::ListSequence(const LinkedList<T>& list) {
    items = new LinkedList<T>(list);
}

template <class T>
ListSequence<T>::ListSequence(const ListSequence<T>& other) {
    items = new LinkedList<T>(*other.items);
}

template <class T>
ListSequence<T>::~ListSequence() {
    delete items;
}

template <class T>
T ListSequence<T>::Get(size_t index) const {
    return items->Get(index);
}

template <class T>
size_t ListSequence<T>::GetCount() const {
    return items->GetLength();
}

template <class T>
Sequence<T>* ListSequence<T>::Clone() const {
    ListSequence<T>* clone = CreateEmpty();
    IEnumerator<T>* en = this->GetEnumerator();
    while (en->MoveNext()) {
        clone->items->Append(en->Current());
    }
    delete en;
    return clone;
}

template <class T>
T ListSequence<T>::GetFirst() const {
    return items->GetFirst();
}

template <class T>
T ListSequence<T>::GetLast() const {
    return items->GetLast();
}

template <class T>
Sequence<T>* ListSequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    LinkedList<T>* sub = items->GetSubList(startIndex, endIndex);
    ListSequence<T>* res = CreateEmpty();
    delete res->items;
    res->items = new LinkedList<T>(*sub);
    delete sub;
    return res;
}

template <class T>
int ListSequence<T>::GetLength() const {
    return items->GetLength();
}

template <class T>
Sequence<T>* ListSequence<T>::Append(const T& item) {
    return ((ListSequence<T>*)Instance())->AppendInternal(item);
}

template <class T>
Sequence<T>* ListSequence<T>::Prepend(const T& item) {
    return ((ListSequence<T>*)Instance())->PrependInternal(item);
}

template <class T>
Sequence<T>* ListSequence<T>::InsertAt(const T& item, int index) {
    return ((ListSequence<T>*)Instance())->InsertAtInternal(item, index);
}

template <class T>
void ListSequence<T>::Set(size_t index, const T& value) {
    ((ListSequence<T>*)Instance())->SetInternal(index, value);
}

template <class T>
Sequence<T>* ListSequence<T>::Concat(const Sequence<T>& list) {
    ListSequence<T>* result = CreateEmpty();
    IEnumerator<T>* enThis = this->GetEnumerator();
    while (enThis->MoveNext()) 
        result->items->Append(enThis->Current());
    delete enThis;

    IEnumerator<T>* enList = list.GetEnumerator();
    while (enList->MoveNext()) 
        result->items->Append(enList->Current());
    delete enList;
    
    return result;
}

template <class T>
template <class R>
Sequence<R>* ListSequence<T>::Map(std::function<R(T)> func) const {
    R* arr = new R[items->GetLength()];
    for (int i = 0; i < items->GetLength(); i++) 
        arr[i] = func(items->Get(i));
    Sequence<R>* res = new MutableListSequence<R>(arr, items->GetLength());
    delete[] arr;
    return res;
}

template <class T>
Sequence<T>* ListSequence<T>::Where(std::function<bool(T)> func) const {
    LinkedList<T>* newList = new LinkedList<T>();
    for (int i = 0; i < items->GetLength(); i++) {
        T val = items->Get(i);
        if (func(val)) newList->Append(val);
    }
    ListSequence<T>* res = CreateEmpty();
    delete res->items;
    res->items = newList;
    return res;
}

template <class T>
T ListSequence<T>::Reduce(std::function<T(T, T)> func, T start) const {
    T result = start;
    IEnumerator<T>* en = this->GetEnumerator();
    while (en->MoveNext()) 
        result = func(result, en->Current());
    delete en;
    return result;
}

template <class T>
Option<T> ListSequence<T>::TryGet(int index) const {
    try {
        return Option<T>::Some(Get(index));
    } catch (...) {
        return Option<T>::None();
    }
}

template <class T>
IEnumerator<T>* ListSequence<T>::GetEnumerator() const {
    return new ListEnumerator<T>(this);
}

template <class T>
T ListSequence<T>::operator[](int index) const {
    return Get(index);
}

template <class T>
MutableListSequence<T>::MutableListSequence() : ListSequence<T>() {
}

template <class T>
MutableListSequence<T>::MutableListSequence(T* items, int count) : ListSequence<T>(items, count) {
}

template <class T>
Sequence<T>* MutableListSequence<T>::Clone() const {
    return new MutableListSequence<T>(*this);
}

template <class T>
Sequence<T>* MutableListSequence<T>::Instance() {
    return this;
}

template <class T>
ListSequence<T>* MutableListSequence<T>::CreateEmpty() const {
    return new MutableListSequence<T>();
}

template <class T>
ImmutableListSequence<T>::ImmutableListSequence() : ListSequence<T>() {
}

template <class T>
ImmutableListSequence<T>::ImmutableListSequence(T* items, int count) : ListSequence<T>(items, count) {
}

template <class T>
Sequence<T>* ImmutableListSequence<T>::Clone() const {
    return new ImmutableListSequence<T>(*this);
}

template <class T>
Sequence<T>* ImmutableListSequence<T>::Instance() {
    return this->Clone();
}

template <class T>
ListSequence<T>* ImmutableListSequence<T>::CreateEmpty() const {
    return new ImmutableListSequence<T>();
}