#pragma once
#include <functional>

#include "icollection.h"
#include "option.h"

template <class T>
class IEnumerator;

template <class T>
class Sequence : public ICollection<T> {
   public:
    virtual ~Sequence() {}

    virtual T Get(size_t index) const override = 0;
    virtual size_t GetCount() const override = 0;

    virtual Sequence<T>* Clone() const override = 0;

    virtual T GetFirst() const = 0;
    virtual T GetLast() const = 0;
    virtual Sequence<T>* GetSubsequence(int startIndex, int endIndex) const = 0;
    virtual int GetLength() const = 0;

    virtual Sequence<T>* Append(T item) = 0;
    virtual Sequence<T>* Prepend(T item) = 0;
    virtual Sequence<T>* InsertAt(T item, int index) = 0;
    virtual Sequence<T>* Concat(Sequence<T>* list) = 0;

    template <class R>
    Sequence<R>* Map(std::function<R(T)> func) const;

    virtual Sequence<T>* Where(std::function<bool(T)> func) const = 0;
    virtual T Reduce(std::function<T(T, T)> func, T start) const = 0;

    virtual Option<T> TryGet(int index) const = 0;
    virtual IEnumerator<T>* GetEnumerator() const = 0;
    virtual T operator[](int index) const = 0;
};