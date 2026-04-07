#pragma once
#include "dynamic_array.h"
#include "iterators.h"
#include "linked_list.h"
#include "sequence.h"

template <class T>
class ArrayEnumerator;

template <class T>
class MutableArraySequence;

template <class T>
class ImmutableArraySequence;

template <class T>
class ArraySequence : public Sequence<T> {
   protected:
    DynamicArray<T>* items;

    virtual ArraySequence<T>* CreateEmpty() const = 0;

    Sequence<T>* AppendInternal(const T& item);
    Sequence<T>* PrependInternal(const T& item);
    Sequence<T>* InsertAtInternal(const T& item, int index);

    void SetInternal(size_t index, const T& value);

   public:
    ArraySequence();
    ArraySequence(T* itemsArr, int count);
    ArraySequence(const LinkedList<T>& list);
    ArraySequence(const ArraySequence<T>& other);
    virtual ~ArraySequence();

    T Get(size_t index) const override;
    size_t GetCount() const override;
    Sequence<T>* Clone() const override;
    T GetFirst() const override;
    T GetLast() const override;
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override;
    int GetLength() const override;
    IEnumerator<T>* GetEnumerator() const override;


    Sequence<T>* Append(const T& item) override;
    Sequence<T>* Prepend(const T& item) override;
    Sequence<T>* InsertAt(const T& item, int index) override;
    void Set(size_t index, const T& value) override;

    virtual Sequence<T>* Instance() = 0;

    Sequence<T>* Concat(Sequence<T>* list) override;
    template <class R>
    Sequence<R>* Map(std::function<R(T)> func) const;
    Sequence<T>* Where(std::function<bool(T)> func) const override;
    T Reduce(std::function<T(T, T)> func, T start) const override;

    Option<T> TryGet(int index) const override;
    
    T operator[](int index) const override;
};

template <class T>
class MutableArraySequence : public ArraySequence<T> {
   public:
    MutableArraySequence();
    MutableArraySequence(T* items, int count);
    MutableArraySequence(const MutableArraySequence<T>& other);

    Sequence<T>* Clone() const override;
    Sequence<T>* Instance() override;
    ArraySequence<T>* CreateEmpty() const override;
};

template <class T>
class ImmutableArraySequence : public ArraySequence<T> {
   public:
    ImmutableArraySequence();
    ImmutableArraySequence(T* items, int count);
    ImmutableArraySequence(const ImmutableArraySequence<T>& other);

    Sequence<T>* Clone() const override;
    Sequence<T>* Instance() override;
    ArraySequence<T>* CreateEmpty() const override;
};

#include "array_sequence.tpp"