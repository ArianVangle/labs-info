#pragma once

#include "dynamic_array.h"
#include "iterators.h"
#include "linked_list.h"
#include "sequence.h"

template <class T>
class ArrayEnumerator;

template <class T>
class ArraySequence : public Sequence<T> {
   protected:
    DynamicArray<T>* items;

   public:
    ArraySequence() { items = new DynamicArray<T>(0); }

    ArraySequence(T* itemsArr, int count) {
        items = new DynamicArray<T>(itemsArr, count);
    }

    ArraySequence(const LinkedList<T>& list) {
        items = new DynamicArray<T>(list.GetLength());
        for (int i = 0; i < list.GetLength(); i++) {
            items->Set(i, list.Get(i));
        }
    }

    ArraySequence(const ArraySequence<T>& other) {
        items = new DynamicArray<T>(*other.items);
    }

    virtual ~ArraySequence() { delete items; }

    T Get(size_t index) const override { return items->Get(index); }

    size_t GetCount() const override { return items->GetSize(); }

    Sequence<T>* Clone() const override { return new ArraySequence<T>(*this); }

    T GetFirst() const override {
        if (items->GetSize() == 0)
            throw IndexOutOfRangeException("Sequence is empty");
        return items->Get(0);
    }

    T GetLast() const override {
        if (items->GetSize() == 0)
            throw IndexOutOfRangeException("Sequence is empty");
        return items->Get(items->GetSize() - 1);
    }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override {
        if (startIndex < 0 || endIndex >= items->GetSize() ||
            startIndex > endIndex) {
            throw IndexOutOfRangeException("Invalid indices");
        }
        int len = endIndex - startIndex + 1;
        T* subItems = new T[len];
        for (int i = 0; i < len; i++) {
            subItems[i] = items->Get(startIndex + i);
        }
        Sequence<T>* res = new ArraySequence<T>(subItems, len);
        delete[] subItems;
        return res;
    }

    int GetLength() const override { return items->GetSize(); }

    Sequence<T>* Append(T item) override {
        items->Resize(items->GetSize() + 1);
        items->Set(items->GetSize() - 1, item);
        return this;
    }

    Sequence<T>* Prepend(T item) override {
        DynamicArray<T>* newItems = new DynamicArray<T>(items->GetSize() + 1);
        newItems->Set(0, item);
        for (int i = 0; i < items->GetSize(); i++) {
            newItems->Set(i + 1, items->Get(i));
        }
        delete items;
        items = newItems;
        return this;
    }

    Sequence<T>* InsertAt(T item, int index) override {
        if (index < 0 || index > items->GetSize())
            throw IndexOutOfRangeException("Index out of range");
        DynamicArray<T>* newItems = new DynamicArray<T>(items->GetSize() + 1);
        for (int i = 0; i < index; i++) newItems->Set(i, items->Get(i));
        newItems->Set(index, item);
        for (int i = index; i < items->GetSize(); i++)
            newItems->Set(i + 1, items->Get(i));
        delete items;
        items = newItems;
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
        R* newArr = new R[items->GetSize()];
        for (int i = 0; i < items->GetSize(); i++) {
            newArr[i] = func(items->Get(i));
        }
        Sequence<R>* res = new ArraySequence<R>(newArr, items->GetSize());
        delete[] newArr;
        return res;
    }

    Sequence<T>* Where(std::function<bool(T)> func) const override {
        int count = 0;
        for (int i = 0; i < items->GetSize(); i++) {
            if (func(items->Get(i))) count++;
        }
        T* newArr = new T[count];
        int idx = 0;
        for (int i = 0; i < items->GetSize(); i++) {
            if (func(items->Get(i))) newArr[idx++] = items->Get(i);
        }
        Sequence<T>* res = new ArraySequence<T>(newArr, count);
        delete[] newArr;
        return res;
    }

    T Reduce(std::function<T(T, T)> func, T start) const override {
        T res = start;
        for (int i = 0; i < items->GetSize(); i++) {
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
        return new ArrayEnumerator<T>(this);
    }

    T operator[](int index) const override { return Get(index); }
};

// Mutable Implementation
template <class T>
class MutableArraySequence : public ArraySequence<T> {
   public:
    MutableArraySequence() : ArraySequence<T>() {}
    MutableArraySequence(T* items, int count)
        : ArraySequence<T>(items, count) {}
    MutableArraySequence(const MutableArraySequence<T>& other)
        : ArraySequence<T>(other) {}

    Sequence<T>* Clone() const override {
        return new MutableArraySequence<T>(*this);
    }
};

// Immutable Implementation
template <class T>
class ImmutableArraySequence : public ArraySequence<T> {
   public:
    ImmutableArraySequence() : ArraySequence<T>() {}
    ImmutableArraySequence(T* items, int count)
        : ArraySequence<T>(items, count) {}
    ImmutableArraySequence(const ImmutableArraySequence<T>& other)
        : ArraySequence<T>(other) {}

    Sequence<T>* Append(T item) override {
        ImmutableArraySequence<T>* newSeq = new ImmutableArraySequence<T>();
        newSeq->items->Resize(this->GetLength() + 1);
        for (int i = 0; i < this->GetLength(); i++) {
            newSeq->items->Set(i, this->Get(i));
        }
        newSeq->items->Set(this->GetLength(), item);
        return newSeq;
    }

    Sequence<T>* Clone() const override {
        return new ImmutableArraySequence<T>(*this);
    }
};