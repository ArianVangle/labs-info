#pragma once
#include "iterators.h"
#include "linked_list.h"
#include "sequence.h"

template <class T>
class ListEnumerator;

template <class T>
class MutableListSequence;

template <class T>
class ImmutableListSequence;

template <class T>
class ListSequence : public Sequence<T> {
   protected:
    LinkedList<T>* items;

    virtual ListSequence<T>* CreateEmpty() const = 0;

    Sequence<T>* AppendInternal(const T& item);
    Sequence<T>* PrependInternal(const T& item);
    Sequence<T>* InsertAtInternal(const T& item, int index);

    void SetInternal(size_t index, const T& value);

   public:
    ListSequence();
    ListSequence(T* itemsArr, int count);
    ListSequence(const LinkedList<T>& list);
    ListSequence(const ListSequence<T>& other);
    virtual ~ListSequence();

    T Get(size_t index) const override;
    size_t GetCount() const override;
    T GetFirst() const override;
    T GetLast() const override;
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override;
    int GetLength() const override;

    Sequence<T>* Append(const T& item) override;
    Sequence<T>* Prepend(const T& item) override;
    Sequence<T>* InsertAt(const T& item, int index) override;
    void Set(size_t index, const T& value) override;
    
    virtual Sequence<T>* Instance() = 0;

    Sequence<T>* Clone() const override;
    Sequence<T>* Concat(const Sequence<T>& list) override;
    template <class R>
    Sequence<R>* Map(std::function<R(T)> func) const;
    Sequence<T>* Where(std::function<bool(T)> func) const override;
    T Reduce(std::function<T(T, T)> func, T start) const override;

    Option<T> TryGet(int index) const override;
    IEnumerator<T>* GetEnumerator() const override;
    T operator[](int index) const override;
};

template <class T>
class MutableListSequence : public ListSequence<T> {
   public:
    MutableListSequence();
    MutableListSequence(T* items, int count);

    Sequence<T>* Clone() const override;
    Sequence<T>* Instance() override;
    ListSequence<T>* CreateEmpty() const override;
};

template <class T>
class ImmutableListSequence : public ListSequence<T> {
   public:
    ImmutableListSequence();
    ImmutableListSequence(T* items, int count);

    Sequence<T>* Clone() const override;
    Sequence<T>* Instance() override;
    ListSequence<T>* CreateEmpty() const override;
};

#include "list_sequence.tpp"