#pragma once
#include "iterators.h"

template <class T, class Derived>
ArrayEnumerator<T, Derived>::ArrayEnumerator(const ArraySequence<T, Derived>* s) : seq(s), index(-1) {}

template <class T, class Derived>
ArrayEnumerator<T, Derived>::~ArrayEnumerator() {}

template <class T, class Derived>
T ArrayEnumerator<T, Derived>::Current() const {
    return seq->Get(index);
}

template <class T, class Derived>
bool ArrayEnumerator<T, Derived>::MoveNext() {
    index++;
    return index < seq->GetLength();
}

template <class T, class Derived>
void ArrayEnumerator<T, Derived>::Reset() {
    index = -1;
}

template <class T, class Derived>
ListEnumerator<T, Derived>::ListEnumerator(const ListSequence<T, Derived>* s) : seq(s), index(-1) {}

template <class T, class Derived>
ListEnumerator<T, Derived>::~ListEnumerator() {}

template <class T, class Derived>
T ListEnumerator<T, Derived>::Current() const {
    return seq->Get(index);
}

template <class T, class Derived>
bool ListEnumerator<T, Derived>::MoveNext() {
    index++;
    return index < seq->GetLength();
}

template <class T, class Derived>
void ListEnumerator<T, Derived>::Reset() {
    index = -1;
}