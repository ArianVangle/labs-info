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

    Sequence<T>* AppendInternal(T item) {
        items->Resize(items->GetSize() + 1);
        items->Set(items->GetSize() - 1, item);
        return this;
    }

    Sequence<T>* PrependInternal(T item) {
        DynamicArray<T>* newItems = new DynamicArray<T>(items->GetSize() + 1);
        newItems->Set(0, item);
        for (int i = 0; i < items->GetSize(); i++) {
            newItems->Set(i + 1, items->Get(i));
        }
        delete items;
        items = newItems;
        return this;
    }

    Sequence<T>* InsertAtInternal(T item, int index) {
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

    void SetInternal(size_t index, T value) { items->Set(index, value); }

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

    Sequence<T>* Clone() const override {
        ArraySequence<T>* clone = CreateEmpty();
        for (int i = 0; i < items->GetSize(); i++) {
            clone->items->Set(i, items->Get(i));
        }
        return clone;
    }

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
        ArraySequence<T>* res = CreateEmpty();
        res->items->Resize(len);
        for (int i = 0; i < len; i++) {
            res->items->Set(i, items->Get(startIndex + i));
        }
        return res;
    }

    int GetLength() const override { return items->GetSize(); }

    Sequence<T>* Append(T item) override {
        return ((ArraySequence<T>*)Instance())->AppendInternal(item);
    }

    Sequence<T>* Prepend(T item) override {
        return ((ArraySequence<T>*)Instance())->PrependInternal(item);
    }

    Sequence<T>* InsertAt(T item, int index) override {
        return ((ArraySequence<T>*)Instance())->InsertAtInternal(item, index);
    }

    void Set(size_t index, T value) override {
        ((ArraySequence<T>*)Instance())->SetInternal(index, value);
    }

    virtual Sequence<T>* Instance() = 0;

    Sequence<T>* Concat(Sequence<T>* list) override {
        int currentLen = items->GetSize();
        int addLen = list->GetLength();
        items->Resize(currentLen + addLen);

        IEnumerator<T>* en = list->GetEnumerator();
        int i = currentLen;
        while (en->MoveNext()) {
            items->Set(i++, en->Current());
        }
        delete en;
        return this;
    }

    template <class R>
    Sequence<R>* Map(std::function<R(T)> func) const {
        R* newArr = new R[items->GetSize()];
        for (int i = 0; i < items->GetSize(); i++) {
            newArr[i] = func(items->Get(i));
        }
        Sequence<R>* res =
            new MutableArraySequence<R>(newArr, items->GetSize());
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
        ArraySequence<T>* res = CreateEmpty();
        res->items->Resize(count);
        for (int i = 0; i < count; i++) {
            res->items->Set(i, newArr[i]);
        }
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

    Sequence<T>* Instance() override { return this; }

    ArraySequence<T>* CreateEmpty() const override {
        return new MutableArraySequence<T>();
    }
};

template <class T>
class ImmutableArraySequence : public ArraySequence<T> {
   public:
    ImmutableArraySequence() : ArraySequence<T>() {}
    ImmutableArraySequence(T* items, int count)
        : ArraySequence<T>(items, count) {}
    ImmutableArraySequence(const ImmutableArraySequence<T>& other)
        : ArraySequence<T>(other) {}

    Sequence<T>* Clone() const override {
        return new ImmutableArraySequence<T>(*this);
    }

    Sequence<T>* Instance() override { return this->Clone(); }

    ArraySequence<T>* CreateEmpty() const override {
        return new ImmutableArraySequence<T>();
    }
};