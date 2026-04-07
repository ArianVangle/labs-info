#pragma once
#include "iterators.h"

template <class T>
ArrayEnumerator<T>::ArrayEnumerator(const ArraySequence<T>* s) : seq(s), index(-1) {}

template <class T>
ArrayEnumerator<T>::~ArrayEnumerator() {}

template <class T>
T ArrayEnumerator<T>::Current() const {
    return seq->Get(index);
}

template <class T>
bool ArrayEnumerator<T>::MoveNext() {
    index++;
    return index < seq->GetLength();
}

template <class T>
void ArrayEnumerator<T>::Reset() {
    index = -1;
}

template <class T>
ListEnumerator<T>::ListEnumerator(const ListSequence<T>* s) : seq(s), index(-1) {}

template <class T>
ListEnumerator<T>::~ListEnumerator() {}

template <class T>
T ListEnumerator<T>::Current() const {
    return seq->Get(index);
}

template <class T>
bool ListEnumerator<T>::MoveNext() {
    index++;
    return index < seq->GetLength();
}

template <class T>
void ListEnumerator<T>::Reset() {
    index = -1;
}