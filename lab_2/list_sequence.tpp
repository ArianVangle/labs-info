#pragma once
#include "list_sequence.h"


template <class T, class Derived>
void ListSequence<T, Derived>::AppendInternal(const T& item) {
    items->Append(item);
}

template <class T, class Derived>
void ListSequence<T, Derived>::PrependInternal(const T& item) {
    items->Prepend(item);
}

template <class T, class Derived>
void ListSequence<T, Derived>::InsertAtInternal(const T& item, int index) {
    items->InsertAt(item, index);
}

template <class T, class Derived>
void ListSequence<T, Derived>::SetInternal(size_t index, const T& value) {
    items->Set(index, value);
}

template <class T, class Derived>
ListSequence<T, Derived>::ListSequence() {
    items = new LinkedList<T>();
}

template <class T, class Derived>
ListSequence<T, Derived>::ListSequence(T* itemsArr, int count) {
    items = new LinkedList<T>(itemsArr, count);
}

template <class T, class Derived>
ListSequence<T, Derived>::ListSequence(const LinkedList<T>& list) {
    items = new LinkedList<T>(list);
}

template <class T, class Derived>
ListSequence<T, Derived>::ListSequence(const ListSequence& other) {
    items = new LinkedList<T>(*other.items);
}

template <class T, class Derived>
ListSequence<T, Derived>::~ListSequence() {
    delete items;
}

template <class T, class Derived>
T ListSequence<T, Derived>::Get(size_t index) const {
    return items->Get(index);
}

template <class T, class Derived>
size_t ListSequence<T, Derived>::GetCount() const {
    return items->GetLength();
}

template <class T, class Derived>
int ListSequence<T, Derived>::GetLength() const {
    return items->GetLength();
}

template <class T, class Derived>
T ListSequence<T, Derived>::GetFirst() const {
    return items->GetFirst();
}

template <class T, class Derived>
T ListSequence<T, Derived>::GetLast() const {
    return items->GetLast();
}

template <class T, class Derived>
IEnumerator<T>* ListSequence<T, Derived>::GetEnumerator() const {
    return new ListEnumerator<T, Derived>(this);
}

template <class T, class Derived>
Sequence<T>* ListSequence<T, Derived>::Clone() const {
    return new Derived(static_cast<const Derived&>(*this));
}

template <class T, class Derived>
Sequence<T>* ListSequence<T, Derived>::GetSubsequence(int startIndex, int endIndex) const {
    LinkedList<T>* sub = items->GetSubList(startIndex, endIndex);
    auto* res = new MutableListSequence<T>();
    delete res->items;
    res->items = new LinkedList<T>(*sub);
    delete sub;
    return res;
}

template <class T, class Derived>
Sequence<T>* ListSequence<T, Derived>::Concat(const Sequence<T>& list) {
    auto* res = new MutableListSequence<T>();
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
Sequence<R>* ListSequence<T, Derived>::Map(std::function<R(T)> func) const {
    auto* res = new MutableArraySequence<R>();
    IEnumerator<T>* en = this->GetEnumerator();
    while (en->MoveNext()) res->Append(func(en->Current()));
    delete en;
    return res;
}

template <class T, class Derived>
Sequence<T>* ListSequence<T, Derived>::Where(std::function<bool(T)> func) const {
    auto* res = new MutableListSequence<T>();
    IEnumerator<T>* en = this->GetEnumerator();
    while (en->MoveNext()) {
        if (func(en->Current())) res->Append(en->Current());
    }
    delete en;
    return res;
}

template <class T, class Derived>
T ListSequence<T, Derived>::Reduce(std::function<T(T, T)> func, T start) const {
    T result = start;
    IEnumerator<T>* en = this->GetEnumerator();
    while (en->MoveNext()) result = func(result, en->Current());
    delete en;
    return result;
}

template <class T, class Derived>
Option<T> ListSequence<T, Derived>::TryGet(int index) const {
    try { return Option<T>::Some(Get(index)); }
    catch (...) { return Option<T>::None(); }
}

template <class T, class Derived>
T ListSequence<T, Derived>::operator[](int index) const {
    return Get(index);
}


template <class T>
MutableListSequence<T>::MutableListSequence() : ListSequence<T, MutableListSequence<T>>() {}

template <class T>
MutableListSequence<T>::MutableListSequence(T* items, int count) : ListSequence<T, MutableListSequence<T>>(items, count) {}

template <class T>
Sequence<T>* MutableListSequence<T>::Clone() const {
    return new MutableListSequence<T>(*this);
}

template <class T>
Sequence<T>* MutableListSequence<T>::Append(const T& item) {
    this->AppendInternal(item);
    return this;
}

template <class T>
Sequence<T>* MutableListSequence<T>::Prepend(const T& item) {
    this->PrependInternal(item);
    return this;
}

template <class T>
Sequence<T>* MutableListSequence<T>::InsertAt(const T& item, int index) {
    this->InsertAtInternal(item, index);
    return this;
}

template <class T>
void MutableListSequence<T>::Set(size_t index, const T& value) {
    this->SetInternal(index, value);
}


template <class T>
ImmutableListSequence<T>::ImmutableListSequence() : ListSequence<T, ImmutableListSequence<T>>() {}

template <class T>
ImmutableListSequence<T>::ImmutableListSequence(T* items, int count) : ListSequence<T, ImmutableListSequence<T>>(items, count) {}

template <class T>
Sequence<T>* ImmutableListSequence<T>::Clone() const {
    return new ImmutableListSequence<T>(*this);
}

template <class T>
Sequence<T>* ImmutableListSequence<T>::Append(const T& item) {
    auto* clone = static_cast<ImmutableListSequence<T>*>(this->Clone());
    clone->AppendInternal(item);
    return clone;
}

template <class T>
Sequence<T>* ImmutableListSequence<T>::Prepend(const T& item) {
    auto* clone = static_cast<ImmutableListSequence<T>*>(this->Clone());
    clone->PrependInternal(item);
    return clone;
}

template <class T>
Sequence<T>* ImmutableListSequence<T>::InsertAt(const T& item, int index) {
    auto* clone = static_cast<ImmutableListSequence<T>*>(this->Clone());
    clone->InsertAtInternal(item, index);
    return clone;
}

template <class T>
void ImmutableListSequence<T>::Set(size_t index, const T& value) {
    throw InvalidOperationException("Cannot modify immutable sequence");
}