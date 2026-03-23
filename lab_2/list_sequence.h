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

    Sequence<T>* AppendInternal(T item) {
        items->Append(item);
        return this;
    }

    Sequence<T>* PrependInternal(T item) {
        items->Prepend(item);
        return this;
    }

    Sequence<T>* InsertAtInternal(T item, int index) {
        items->InsertAt(item, index);
        return this;
    }

    void SetInternal(size_t index, T value) { items->Set(index, value); }

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

    Sequence<T>* Clone() const override {
        ListSequence<T>* clone = CreateEmpty();
        IEnumerator<T>* en = this->GetEnumerator();
        while (en->MoveNext()) {
            clone->items->Append(en->Current());
        }
        delete en;
        return clone;
    }

    T GetFirst() const override { return items->GetFirst(); }
    T GetLast() const override { return items->GetLast(); }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override {
        LinkedList<T>* sub = items->GetSubList(startIndex, endIndex);
        ListSequence<T>* res = CreateEmpty();
        delete res->items;
        res->items = new LinkedList<T>(*sub);
        delete sub;
        return res;
    }

    int GetLength() const override { return items->GetLength(); }

    Sequence<T>* Append(T item) override {
        return ((ListSequence<T>*)Instance())->AppendInternal(item);
    }

    Sequence<T>* Prepend(T item) override {
        return ((ListSequence<T>*)Instance())->PrependInternal(item);
    }

    Sequence<T>* InsertAt(T item, int index) override {
        return ((ListSequence<T>*)Instance())->InsertAtInternal(item, index);
    }

    void Set(size_t index, T value) override {
        ((ListSequence<T>*)Instance())->SetInternal(index, value);
    }

    virtual Sequence<T>* Instance() = 0;

    Sequence<T>* Concat(Sequence<T>* list) override {
        IEnumerator<T>* en = list->GetEnumerator();
        while (en->MoveNext()) {
            items->Append(en->Current());
        }
        delete en;
        return this;
    }

    template <class R>
    Sequence<R>* Map(std::function<R(T)> func) const {
        R* arr = new R[items->GetLength()];
        for (int i = 0; i < items->GetLength(); i++) {
            arr[i] = func(items->Get(i));
        }
        Sequence<R>* res = new MutableListSequence<R>(arr, items->GetLength());
        delete[] arr;
        return res;
    }

    Sequence<T>* Where(std::function<bool(T)> func) const override {
        LinkedList<T>* newList = new LinkedList<T>();
        for (int i = 0; i < items->GetLength(); i++) {
            T val = items->Get(i);
            if (func(val)) newList->Append(val);
        }
        ListSequence<T>* res = CreateEmpty();
        delete res->items;
        res->items = newList;
        return res;
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

    Sequence<T>* Instance() override { return this; }

    ListSequence<T>* CreateEmpty() const override {
        return new MutableListSequence<T>();
    }
};

template <class T>
class ImmutableListSequence : public ListSequence<T> {
   public:
    ImmutableListSequence() : ListSequence<T>() {}
    ImmutableListSequence(T* items, int count)
        : ListSequence<T>(items, count) {}

    Sequence<T>* Clone() const override {
        return new ImmutableListSequence<T>(*this);
    }

    Sequence<T>* Instance() override { return this->Clone(); }

    ListSequence<T>* CreateEmpty() const override {
        return new ImmutableListSequence<T>();
    }
};