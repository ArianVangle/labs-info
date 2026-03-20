#pragma once
#include "iterators.h"
#include "linked_list.h"
#include "sequence.h"

template <class T>
class ListEnumerator;

template <class T>
class ListSequence : public Sequence<T> {
   protected:
    LinkedList<T>* items;

   public:
    ListSequence() { items = new LinkedList<T>(); }

    ListSequence(T* itemsArr, int count) {
        items = new LinkedList<T>(itemsArr, count);
    }

    ListSequence(const LinkedList<T>& list) { items = new LinkedList<T>(list); }

    ListSequence(const ListSequence<T>& other) {
        items = new LinkedList<T>(*other.items);
    }

    virtual ~ListSequence() { delete items; }

    T Get(size_t index) const override { return items->Get(index); }

    size_t GetCount() const override { return items->GetLength(); }

    Sequence<T>* Clone() const override { return new ListSequence<T>(*this); }

    T GetFirst() const override { return items->GetFirst(); }
    T GetLast() const override { return items->GetLast(); }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override {
        LinkedList<T>* sub = items->GetSubList(startIndex, endIndex);
        return new ListSequence<T>(*sub);
    }

    int GetLength() const override { return items->GetLength(); }

    Sequence<T>* Append(T item) override {
        items->Append(item);
        return this;
    }
    Sequence<T>* Prepend(T item) override {
        items->Prepend(item);
        return this;
    }
    Sequence<T>* InsertAt(T item, int index) override {
        items->InsertAt(item, index);
        return this;
    }
    Sequence<T>* Concat(Sequence<T>* list) override {
        for (int i = 0; i < list->GetLength(); i++) {
            Append(list->Get(i));
        }
        return this;
    }

    template <class R>
    Sequence<R>* Map(std::function<R(T)> func) const {
        R* arr = new R[items->GetLength()];
        for (int i = 0; i < items->GetLength(); i++)
            arr[i] = func(items->Get(i));
        Sequence<R>* res = new ListSequence<R>(arr, items->GetLength());
        delete[] arr;
        return res;
    }

    Sequence<T>* Where(std::function<bool(T)> func) const override {
        LinkedList<T>* newList = new LinkedList<T>();
        for (int i = 0; i < items->GetLength(); i++) {
            T val = items->Get(i);
            if (func(val)) newList->Append(val);
        }
        return new ListSequence<T>(*newList);
    }

    T Reduce(std::function<T(T, T)> func, T start) const override {
        T res = start;
        for (int i = 0; i < items->GetLength(); i++) {
            res = func(res, items->Get(i));
        }
        return res;
    }

    Option<T> TryGet(int index) const override {
        try {
            return Option<T>::Some(Get(index));
        } catch (...) {
            return Option<T>::None();
        }
    }

    IEnumerator<T>* GetEnumerator() const override {
        return new ListEnumerator<T>(this);
    }

    T operator[](int index) const override { return Get(index); }
};

template <class T>
class MutableListSequence : public ListSequence<T> {
   public:
    MutableListSequence() : ListSequence<T>() {}
    MutableListSequence(T* items, int count) : ListSequence<T>(items, count) {}
    Sequence<T>* Clone() const override {
        return new MutableListSequence<T>(*this);
    }
};

template <class T>
class ImmutableListSequence : public ListSequence<T> {
   public:
    ImmutableListSequence() : ListSequence<T>() {}
    ImmutableListSequence(T* items, int count)
        : ListSequence<T>(items, count) {}

    Sequence<T>* Append(T item) override {
        ImmutableListSequence<T>* newSeq = new ImmutableListSequence<T>();
        for (int i = 0; i < this->GetLength(); i++) {
            newSeq->items->Append(this->Get(i));
        }
        newSeq->items->Append(item);
        return newSeq;
    }

    Sequence<T>* Clone() const override {
        return new ImmutableListSequence<T>(*this);
    }
};