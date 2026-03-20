#pragma once
#include <iostream>

template <class T>
class IEnumerator {
   public:
    virtual ~IEnumerator() {}
    virtual T Current() const = 0;
    virtual bool MoveNext() = 0;
    virtual void Reset() = 0;
};

template <class T>
class IEnumerable {
   public:
    virtual ~IEnumerable() {}
    virtual IEnumerator<T>* GetEnumerator() const = 0;
};

template <class T>
class ArraySequence;

template <class T>
class ArrayEnumerator : public IEnumerator<T> {
    const ArraySequence<T>* seq;
    int index;

   public:
    ArrayEnumerator(const ArraySequence<T>* s) : seq(s), index(-1) {}
    ~ArrayEnumerator() {}
    T Current() const override { return seq->Get(index); }
    bool MoveNext() override {
        index++;
        return index < seq->GetLength();
    }
    void Reset() override { index = -1; }
};

template <class T>
class ListSequence;

template <class T>
class ListEnumerator : public IEnumerator<T> {
    const ListSequence<T>* seq;
    int index;

   public:
    ListEnumerator(const ListSequence<T>* s) : seq(s), index(-1) {}
    ~ListEnumerator() {}
    T Current() const override { return seq->Get(index); }
    bool MoveNext() override {
        index++;
        return index < seq->GetLength();
    }
    void Reset() override { index = -1; }
};