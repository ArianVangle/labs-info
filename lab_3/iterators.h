#pragma once

#include "from_lab_2/iterators.h"


template <class T>
class DequeEnumerator;
template <class T>
class DequeSegmented;

template <class T>
class DequeEnumerator : public IEnumerator<T> {
   private:
    const DequeSegmented<T>* deque;
    int currentIndex;

   public:
    DequeEnumerator(const DequeSegmented<T>* d);
    T Current() const override;
    bool MoveNext() override;
    void Reset() override;
};

#include "iterators.tpp"
