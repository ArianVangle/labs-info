#include "iterators.h"

template <class T>
DequeEnumerator<T>::DequeEnumerator(const DequeSegmented<T>* d) : deque(d), currentIndex(-1) {}

template <class T>
T DequeEnumerator<T>::Current() const {
    if (currentIndex < 0 || currentIndex >= deque->GetLength())
        throw InvalidOperationException("Enumerator out of range");
    return deque->Get(currentIndex);
}

template <class T>
bool DequeEnumerator<T>::MoveNext() {
    currentIndex++;
    return currentIndex < deque->GetLength();
}

template <class T>
void DequeEnumerator<T>::Reset() {
    currentIndex = -1;
}