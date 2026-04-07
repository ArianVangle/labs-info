#pragma once

#include <algorithm>
#include <functional>
#include <iostream>

#include "dynamic_array.h"
#include "iterators.h"
#include "option.h"
#include "sequence.h"

template <class T>
class DequeSegmented;

template <class T>
class Segment {
   private:
    T* data;
    int capacity;
    int size;

   public:
    Segment(int cap = 4) : capacity(cap), size(0) { data = new T[capacity]; }

    ~Segment() { delete[] data; }

    Segment(const Segment<T>& other) : capacity(other.capacity), size(other.size) {
        data = new T[capacity];
        for (int i = 0; i < size; i++) {
            data[i] = other.data[i];
        }
    }

    void PushBack(const T& item) {
        if (size >= capacity) throw InvalidOperationException("Segment is full");
        data[size++] = item;
    }

    void PushFront(const T& item) {
        if (size >= capacity) throw InvalidOperationException("Segment is full");
        for (int i = size; i > 0; i--) data[i] = data[i - 1];
        data[0] = item;
        size++;
    }

    T PopBack() {
        if (size == 0) throw InvalidOperationException("Segment is empty");
        return data[--size];
    }

    T PopFront() {
        if (size == 0) throw InvalidOperationException("Segment is empty");
        T value = data[0];
        for (int i = 0; i < size - 1; i++) data[i] = data[i + 1];
        size--;
        return value;
    }

    T Get(int index) const {
        if (index < 0 || index >= size) {
            throw IndexOutOfRangeException("Index out of range in Segment");
        }
        return data[index];
    }

    void Set(int index, const T& value) {
        if (index < 0 || index >= size) {
            throw IndexOutOfRangeException("Index out of range in Segment");
        }
        data[index] = value;
    }

    int GetSize() const { return size; }
    bool IsFull() const { return size >= capacity; }
    bool IsEmpty() const { return size == 0; }
};

template <class T>
class DequeEnumerator : public IEnumerator<T> {
   private:
    const DequeSegmented<T>* deque;
    int currentIndex;

   public:
    DequeEnumerator(const DequeSegmented<T>* d) : deque(d), currentIndex(-1) {}

    T Current() const override {
        if (currentIndex < 0 || currentIndex >= deque->GetLength())
            throw InvalidOperationException("Enumerator out of range");
        return deque->Get(currentIndex);
    }

    bool MoveNext() override {
        currentIndex++;
        return currentIndex < deque->GetLength();
    }

    void Reset() override { currentIndex = -1; }
};

template <class T>
class DequeSegmented : public Sequence<T> {
   private:
    Segment<T>** segments;
    int segmentCapacity;
    int segmentCount;
    int segmentSize;
    int totalCount;
    void EnsureCapacity() {
        if (segmentCount == 0) {
            if (segmentCapacity == 0) {
                segmentCapacity = 4;
                segments = new Segment<T>*[segmentCapacity];
            }
            segments[segmentCount++] = new Segment<T>(segmentSize);
            return;
        }

        if (segments[segmentCount - 1]->IsFull()) {
            if (segmentCount >= segmentCapacity) {
                int newCapacity = segmentCapacity * 2;
                Segment<T>** newSegments = new Segment<T>*[newCapacity];
                for (int i = 0; i < segmentCount; i++) {
                    newSegments[i] = segments[i];
                }
                delete[] segments;
                segments = newSegments;
                segmentCapacity = newCapacity;
            }
            segments[segmentCount++] = new Segment<T>(segmentSize);
        }
    }

    void EnsureFrontCapacity() {
        if (segmentCount == 0) {
            if (segmentCapacity == 0) {
                segmentCapacity = 4;
                segments = new Segment<T>*[segmentCapacity];
            }
            segments[segmentCount++] = new Segment<T>(segmentSize);
            return;
        }

        if (segments[0]->IsFull()) {
            if (segmentCount >= segmentCapacity) {
                int newCapacity = segmentCapacity * 2;
                Segment<T>** newSegments = new Segment<T>*[newCapacity];
                for (int i = 0; i < segmentCount; i++) {
                    newSegments[i + 1] = segments[i];
                }
                delete[] segments;
                segments = newSegments;
                segmentCapacity = newCapacity;
            } else {
                for (int i = segmentCount; i > 0; i--) {
                    segments[i] = segments[i - 1];
                }
            }
            segments[0] = new Segment<T>(segmentSize);
            segmentCount++;
        }
    }

   public:
    DequeSegmented(int segSize = 4) : segmentCapacity(4), segmentCount(0), segmentSize(segSize), totalCount(0) {
        segments = new Segment<T>*[segmentCapacity];
        EnsureCapacity();
    }

    DequeSegmented(const DequeSegmented<T>& other)
        : segmentCapacity(other.segmentCapacity), segmentCount(0), segmentSize(other.segmentSize), totalCount(0) {
        segments = new Segment<T>*[segmentCapacity];
        for (int i = 0; i < other.segmentCount; i++) {
            for (int j = 0; j < other.segments[i]->GetSize(); j++) {
                this->PushBack(other.segments[i]->Get(j));
            }
        }
    }

    virtual ~DequeSegmented() {
        for (int i = 0; i < segmentCount; i++) {
            delete segments[i];
        }
        delete[] segments;
    }

    T operator[](int index) const override { return Get(index); }

    void PushFront(const T& item) {
        EnsureFrontCapacity();
        segments[0]->PushFront(item);
        totalCount++;
    }

    void PushBack(const T& item) {
        EnsureCapacity();
        segments[segmentCount - 1]->PushBack(item);
        totalCount++;
    }

    T PopFront() {
        if (totalCount == 0) throw InvalidOperationException("Deque is empty");
        T value = segments[0]->PopFront();
        if (segments[0]->IsEmpty() && segmentCount > 1) {
            delete segments[0];
            for (int i = 0; i < segmentCount - 1; i++) {
                segments[i] = segments[i + 1];
            }
            segmentCount--;
        }
        totalCount--;
        return value;
    }

    T PopBack() {
        if (totalCount == 0) throw InvalidOperationException("Deque is empty");
        T value = segments[segmentCount - 1]->PopBack();
        if (segments[segmentCount - 1]->IsEmpty() && segmentCount > 1) {
            delete segments[segmentCount - 1];
            segmentCount--;
        }
        totalCount--;
        return value;
    }

    T GetFront() const {
        if (totalCount == 0) throw InvalidOperationException("Deque is empty");
        return segments[0]->Get(0);
    }

    T GetBack() const {
        if (totalCount == 0) throw InvalidOperationException("Deque is empty");
        Segment<T>* last = segments[segmentCount - 1];
        return last->Get(last->GetSize() - 1);
    }
    void Clear() {
        for (int i = 0; i < segmentCount; i++) {
            delete segments[i];
            segments[i] = nullptr;
        }
        segmentCount = 0;
        totalCount = 0;

        EnsureCapacity();
    }

    T Get(size_t index) const override {
        if (totalCount == 0 || index >= (size_t)totalCount) {
            throw IndexOutOfRangeException("Index out of range");
        }

        int remaining = (int)index;
        for (int i = 0; i < segmentCount; i++) {
            if (segments[i] == nullptr) {
                throw IndexOutOfRangeException("Null segment");
            }
            int segSize = segments[i]->GetSize();
            if (remaining < segSize) {
                return segments[i]->Get(remaining);
            }
            remaining -= segSize;
        }

        throw IndexOutOfRangeException("Index out of range (logic error)");
    }

    size_t GetCount() const override { return totalCount; }
    int GetLength() const override { return totalCount; }

    Sequence<T>* Clone() const override { return new DequeSegmented<T>(*this); }

    T GetFirst() const override { return GetFront(); }
    T GetLast() const override { return GetBack(); }

    Sequence<T>* Append(const T& item) override {
        PushBack(item);
        return this;
    }
    Sequence<T>* Prepend(const T& item) override {
        PushFront(item);
        return this;
    }

    Sequence<T>* InsertAt(const T& item, int index) override {
        if (index < 0 || index > totalCount) throw IndexOutOfRangeException("Index out of range");
        DequeSegmented<T>* newDeque = new DequeSegmented<T>(segmentSize);
        for (int i = 0; i < totalCount; i++) {
            if (i == index) newDeque->PushBack(item);
            newDeque->PushBack(Get(i));
        }
        if (index == totalCount) newDeque->PushBack(item);
        return newDeque;
    }

    void Set(size_t index, const T& value) override {
        if (index >= (size_t)totalCount) throw IndexOutOfRangeException("Index out of range");
        int remaining = (int)index;
        for (int i = 0; i < segmentCount; i++) {
            Segment<T>* seg = segments[i];
            int segSize = seg->GetSize();
            if (remaining < segSize) {
                seg->Set(remaining, value);
                return;
            }
            remaining -= segSize;
        }
    }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override {
        if (startIndex < 0 || endIndex >= totalCount || startIndex > endIndex)
            throw IndexOutOfRangeException("Invalid indices");
        DequeSegmented<T>* result = new DequeSegmented<T>(segmentSize);
        for (int i = startIndex; i <= endIndex; i++) {
            result->PushBack(Get(i));
        }
        return result;
    }

    Sequence<T>* Concat(Sequence<T>* list) override {
        DequeSegmented<T>* result = new DequeSegmented<T>(segmentSize);
        IEnumerator<T>* en = this->GetEnumerator();
        while (en->MoveNext()) {
            result->PushBack(en->Current());
        }
        delete en;
        IEnumerator<T>* en2 = list->GetEnumerator();
        while (en2->MoveNext()) {
            result->PushBack(en2->Current());
        }
        delete en2;
        return result;
    }

    void Sort(std::function<bool(T, T)> comparator = [](T a, T b) { return a < b; }) {
        if (totalCount <= 1) return;

        int count = totalCount;
        T** arr = new T*[count];

        for (int i = 0; i < count; i++) {
            arr[i] = new T(Get(i));
        }

        std::sort(arr, arr + count, [comparator](T* a, T* b) { return comparator(*a, *b); });
        Clear();

        for (int i = 0; i < count; i++) {
            PushBack(*arr[i]);
            delete arr[i];
        }

        delete[] arr;
    }

    int FindSubsequence(Sequence<T>* pattern) const {
        if (pattern->GetLength() == 0) return 0;
        if (pattern->GetLength() > totalCount) return -1;
        for (int i = 0; i <= totalCount - pattern->GetLength(); i++) {
            bool found = true;
            for (int j = 0; j < pattern->GetLength(); j++) {
                if (Get(i + j) != pattern->Get(j)) {
                    found = false;
                    break;
                }
            }
            if (found) return i;
        }
        return -1;
    }

    Sequence<T>* Merge(Sequence<T>* other, std::function<bool(T, T)> comparator = [](T a, T b) { return a < b; }) {
        DequeSegmented<T>* result = new DequeSegmented<T>(segmentSize);
        IEnumerator<T>* en1 = this->GetEnumerator();
        IEnumerator<T>* en2 = other->GetEnumerator();
        bool has1 = en1->MoveNext();
        bool has2 = en2->MoveNext();
        while (has1 && has2) {
            if (comparator(en1->Current(), en2->Current())) {
                result->PushBack(en1->Current());
                has1 = en1->MoveNext();
            } else {
                result->PushBack(en2->Current());
                has2 = en2->MoveNext();
            }
        }
        while (has1) {
            result->PushBack(en1->Current());
            has1 = en1->MoveNext();
        }
        while (has2) {
            result->PushBack(en2->Current());
            has2 = en2->MoveNext();
        }
        delete en1;
        delete en2;
        return result;
    }

    template <class R>
    Sequence<R>* Map(std::function<R(T)> func) const {
        DequeSegmented<R>* result = new DequeSegmented<R>(segmentSize);
        IEnumerator<T>* en = this->GetEnumerator();
        while (en->MoveNext()) result->PushBack(func(en->Current()));
        delete en;
        return result;
    }

    Sequence<T>* Where(std::function<bool(T)> func) const override {
        DequeSegmented<T>* result = new DequeSegmented<T>(segmentSize);
        IEnumerator<T>* en = this->GetEnumerator();
        while (en->MoveNext()) {
            if (func(en->Current())) result->PushBack(en->Current());
        }
        delete en;
        return result;
    }

    T Reduce(std::function<T(T, T)> func, T start) const override {
        T result = start;
        IEnumerator<T>* en = this->GetEnumerator();
        while (en->MoveNext()) result = func(result, en->Current());
        delete en;
        return result;
    }

    Option<T> TryGet(int index) const override {
        try {
            return Option<T>::Some(Get(index));
        } catch (...) {
            return Option<T>::None();
        }
    }

    IEnumerator<T>* GetEnumerator() const override { return new DequeEnumerator<T>(this); }

    int GetSegmentCount() const { return segmentCount; }
    int GetSegmentSize() const { return segmentSize; }
    double GetMemoryEfficiency() const {
        int totalCapacity = segmentCount * segmentSize;
        if (totalCapacity == 0) return 0.0;
        return (double)totalCount / totalCapacity * 100.0;
    }
};