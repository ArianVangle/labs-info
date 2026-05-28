#pragma once
#include "iterators.h"
#include "linked_list.h"
#include "sequence.h"

template <class T, class Derived> class ListEnumerator;

template <class T, class Derived>
class ListSequence : public Sequence<T> {
protected:
    LinkedList<T>* items;

    void AppendInternal(const T& item);
    void PrependInternal(const T& item);
    void InsertAtInternal(const T& item, int index);
    void SetInternal(size_t index, const T& value);

public:
    ListSequence();
    ListSequence(T* itemsArr, int count);
    ListSequence(const LinkedList<T>& list);
    ListSequence(const ListSequence& other);
    virtual ~ListSequence();

    T Get(size_t index) const override;
    size_t GetCount() const override;
    int GetLength() const override;
    T GetFirst() const override;
    T GetLast() const override;

    Sequence<T>* Append(const T& item) override {
        return static_cast<Derived*>(this)->Append(item);
    }
    Sequence<T>* Prepend(const T& item) override {
        return static_cast<Derived*>(this)->Prepend(item);
    }
    Sequence<T>* InsertAt(const T& item, int index) override {
        return static_cast<Derived*>(this)->InsertAt(item, index);
    }
    void Set(size_t index, const T& value) override {
        static_cast<Derived*>(this)->Set(index, value);
    }
    Sequence<T>* Clone() const override;
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override;
    Sequence<T>* Concat(const Sequence<T>& list) override;

    IEnumerator<T>* GetEnumerator() const override;
    template <class R> Sequence<R>* Map(std::function<R(T)> func) const;
    Sequence<T>* Where(std::function<bool(T)> func) const override;
    T Reduce(std::function<T(T, T)> func, T start) const override;
    Option<T> TryGet(int index) const override;
    T operator[](int index) const override;
};

template <class T>
class MutableListSequence : public ListSequence<T, MutableListSequence<T>> {
public:
    MutableListSequence();
    MutableListSequence(T* items, int count);
    
    Sequence<T>* Clone() const override;
    Sequence<T>* Append(const T& item) override;
    Sequence<T>* Prepend(const T& item) override;
    Sequence<T>* InsertAt(const T& item, int index) override;
    void Set(size_t index, const T& value) override;
};

template <class T>
class ImmutableListSequence : public ListSequence<T, ImmutableListSequence<T>> {
public:
    ImmutableListSequence();
    ImmutableListSequence(T* items, int count);
    
    Sequence<T>* Clone() const override;
    Sequence<T>* Append(const T& item) override;
    Sequence<T>* Prepend(const T& item) override;
    Sequence<T>* InsertAt(const T& item, int index) override;
    void Set(size_t index, const T& value) override;
};

#include "list_sequence.tpp"