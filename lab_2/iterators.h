#pragma once

template <class T>
class IEnumerator {
   public:
    virtual ~IEnumerator() {
    }
    virtual T Current() const = 0;
    virtual bool MoveNext() = 0;
    virtual void Reset() = 0;
};

template <class T>
class IEnumerable {
   public:
    virtual ~IEnumerable() {
    }
    virtual IEnumerator<T>* GetEnumerator() const = 0;
};


template <class T, class Derived> class ArraySequence;

template <class T, class Derived>
class ArrayEnumerator : public IEnumerator<T> {
    const ArraySequence<T, Derived>* seq;
    int index;

   public:
    ArrayEnumerator(const ArraySequence<T, Derived>* s);
    ~ArrayEnumerator();
    T Current() const override;
    bool MoveNext() override;
    void Reset() override;
};


template <class T, class Derived> class ListSequence;

template <class T, class Derived>
class ListEnumerator : public IEnumerator<T> {
    const ListSequence<T, Derived>* seq;
    int index;

   public:
    ListEnumerator(const ListSequence<T, Derived>* s);
    ~ListEnumerator();
    T Current() const override;
    bool MoveNext() override;
    void Reset() override;
};

#include "iterators.tpp"