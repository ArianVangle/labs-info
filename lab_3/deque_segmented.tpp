#pragma once
#include "deque_segmented.h"

template <class T>
Segment<T>::Segment(int cap) : capacity(cap), size(0) {
    data = new T[capacity];
}

template <class T>
Segment<T>::~Segment() {
    delete[] data;
}

template <class T>
Segment<T>::Segment(const Segment<T>& other) : capacity(other.capacity), size(other.size) {
    data = new T[capacity];
    for (int i = 0; i < size; i++) 
        data[i] = other.data[i];
}

template <class T>
void Segment<T>::PushBack(const T& item) {
    if (size >= capacity) 
        throw InvalidOperationException("Segment is full");
    data[size++] = item;
}

template <class T>
void Segment<T>::PushFront(const T& item) {
    if (size >= capacity)
        throw InvalidOperationException("Segment is full");
    for (int i = size; i > 0; i--) 
        data[i] = data[i - 1];
    data[0] = item;
    size++;
}

template <class T>
T Segment<T>::PopBack() {
    if (size == 0) 
        throw InvalidOperationException("Segment is empty");
    return data[--size];
}

template <class T>
T Segment<T>::PopFront() {
    if (size == 0) 
        throw InvalidOperationException("Segment is empty");
    T value = data[0];
    for (int i = 0; i < size - 1; i++) 
        data[i] = data[i + 1];
    size--;
    return value;
}

template <class T>
T Segment<T>::Get(int index) const {
    if (index < 0 || index >= size) 
        throw IndexOutOfRangeException("Index out of range in Segment");
    return data[index];
}

template <class T>
void Segment<T>::Set(int index, const T& value) {
    if (index < 0 || index >= size) 
        throw IndexOutOfRangeException("Index out of range in Segment");
    data[index] = value;
}

template <class T>
int Segment<T>::GetSize() const {
    return size;
}

template <class T>
bool Segment<T>::IsFull() const {
    return size >= capacity;
}

template <class T>
bool Segment<T>::IsEmpty() const {
    return size == 0;
}

template <class T>
DequeEnumerator<T>::DequeEnumerator(const DequeSegmented<T>* d) : deque(d), currentIndex(-1) {}

template <class T>
T DequeEnumerator<T>::Current() const {
    if (currentIndex < 0 || currentIndex >= deque->GetLength())
        throw InvalidOperationException("Enumerator out of range");
    return deque->Get(currentIndex);
}

template <class T>
bool DequeEnumerator<T>::MoveNext() {
    currentIndex++;
    return currentIndex < deque->GetLength();
}

template <class T>
void DequeEnumerator<T>::Reset() {
    currentIndex = -1;
}

template <class T>
DequeSegmented<T>::DequeSegmented(int segSize)
    : segmentCapacity(4), segmentCount(0), segmentSize(segSize), totalCount(0) {
    segments = new Segment<T>*[segmentCapacity];
    EnsureCapacity();
}

template <class T>
DequeSegmented<T>::DequeSegmented(const DequeSegmented<T>& other)
    : segmentCapacity(other.segmentCapacity), segmentCount(0), segmentSize(other.segmentSize), totalCount(0) {
    segments = new Segment<T>*[segmentCapacity];
    for (int i = 0; i < other.segmentCount; i++)
        for (int j = 0; j < other.segments[i]->GetSize(); j++) 
            this->PushBack(other.segments[i]->Get(j));
}

template <class T>
DequeSegmented<T>::~DequeSegmented() {
    for (int i = 0; i < segmentCount; i++) 
        delete segments[i];
    delete[] segments;
}

template <class T>
void DequeSegmented<T>::EnsureCapacity() {
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
            for (int i = 0; i < segmentCount; i++) 
                newSegments[i] = segments[i];
            delete[] segments;
            segments = newSegments;
            segmentCapacity = newCapacity;
        }
        segments[segmentCount++] = new Segment<T>(segmentSize);
    }
}

template <class T>
void DequeSegmented<T>::EnsureFrontCapacity() {
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
            for (int i = 0; i < segmentCount; i++) 
                newSegments[i + 1] = segments[i];
            delete[] segments;
            segments = newSegments;
            segmentCapacity = newCapacity;
        } else {
            for (int i = segmentCount; i > 0; i--) 
                segments[i] = segments[i - 1];
        }
        segments[0] = new Segment<T>(segmentSize);
        segmentCount++;
    }
}

template <class T>
T DequeSegmented<T>::operator[](int index) const {
    return Get(index);
}

template <class T>
void DequeSegmented<T>::PushFront(const T& item) {
    EnsureFrontCapacity();
    segments[0]->PushFront(item);
    totalCount++;
}

template <class T>
void DequeSegmented<T>::PushBack(const T& item) {
    EnsureCapacity();
    segments[segmentCount - 1]->PushBack(item);
    totalCount++;
}

template <class T>
T DequeSegmented<T>::PopFront() {
    if (totalCount == 0) 
        throw InvalidOperationException("Deque is empty");
    T value = segments[0]->PopFront();
    if (segments[0]->IsEmpty() && segmentCount > 1) {
        delete segments[0];
        for (int i = 0; i < segmentCount - 1; i++) 
            segments[i] = segments[i + 1];
        segmentCount--;
    }
    totalCount--;
    return value;
}

template <class T>
T DequeSegmented<T>::PopBack() {
    if (totalCount == 0)
        throw InvalidOperationException("Deque is empty");
    T value = segments[segmentCount - 1]->PopBack();
    if (segments[segmentCount - 1]->IsEmpty() && segmentCount > 1) {
        delete segments[segmentCount - 1];
        segmentCount--;
    }
    totalCount--;
    return value;
}

template <class T>
T DequeSegmented<T>::GetFront() const {
    if (totalCount == 0) 
        throw InvalidOperationException("Deque is empty");
    return segments[0]->Get(0);
}

template <class T>
T DequeSegmented<T>::GetBack() const {
    if (totalCount == 0) 
        throw InvalidOperationException("Deque is empty");
    Segment<T>* last = segments[segmentCount - 1];
    return last->Get(last->GetSize() - 1);
}

template <class T>
void DequeSegmented<T>::Clear() {
    for (int i = 0; i < segmentCount; i++) {
        delete segments[i];
        segments[i] = nullptr;
    }
    segmentCount = 0;
    totalCount = 0;
    EnsureCapacity();
}

template <class T>
T DequeSegmented<T>::Get(size_t index) const {
    if (totalCount == 0 || index >= (size_t)totalCount) 
        throw IndexOutOfRangeException("Index out of range");
    int remaining = (int)index;
    for (int i = 0; i < segmentCount; i++) {
        if (segments[i] == nullptr) 
            throw IndexOutOfRangeException("Null segment");
        int segSize = segments[i]->GetSize();
        if (remaining < segSize) 
            return segments[i]->Get(remaining);
        remaining -= segSize;
    }
    throw IndexOutOfRangeException("Index out of range (logic error)");
}

template <class T>
size_t DequeSegmented<T>::GetCount() const {
    return totalCount;
}

template <class T>
int DequeSegmented<T>::GetLength() const {
    return totalCount;
}

template <class T>
Sequence<T>* DequeSegmented<T>::Clone() const {
    return new DequeSegmented<T>(*this);
}

template <class T>
T DequeSegmented<T>::GetFirst() const {
    return GetFront();
}

template <class T>
T DequeSegmented<T>::GetLast() const {
    return GetBack();
}

template <class T>
Sequence<T>* DequeSegmented<T>::Append(const T& item) {
    PushBack(item);
    return this;
}

template <class T>
Sequence<T>* DequeSegmented<T>::Prepend(const T& item) {
    PushFront(item);
    return this;
}

template <class T>
Sequence<T>* DequeSegmented<T>::InsertAt(const T& item, int index) {
    if (index < 0 || index > totalCount) 
        throw IndexOutOfRangeException("Index out of range");
    DequeSegmented<T>* newDeque = new DequeSegmented<T>(segmentSize);
    for (int i = 0; i < totalCount; i++) {
        if (i == index) newDeque->PushBack(item);
        newDeque->PushBack(Get(i));
    }
    if (index == totalCount) newDeque->PushBack(item);
    return newDeque;
}

template <class T>
void DequeSegmented<T>::Set(size_t index, const T& value) {
    if (index >= (size_t)totalCount) 
        throw IndexOutOfRangeException("Index out of range");
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

template <class T>
Sequence<T>* DequeSegmented<T>::GetSubsequence(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= totalCount || startIndex > endIndex)
        throw IndexOutOfRangeException("Invalid indices");
    DequeSegmented<T>* result = new DequeSegmented<T>(segmentSize);
    for (int i = startIndex; i <= endIndex; i++) 
        result->PushBack(Get(i));
    return result;
}

template <class T>
Sequence<T>* DequeSegmented<T>::Concat(Sequence<T>* list) {
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

template <class T>
void DequeSegmented<T>::Sort(std::function<bool(T, T)> comparator) {
    if (totalCount <= 1) 
        return;

    int count = totalCount;
    T** arr = new T*[count];
    for (int i = 0; i < count; i++) 
        arr[i] = new T(Get(i));
    std::sort(arr, arr + count, [comparator](T* a, T* b) { return comparator(*a, *b); });
    Clear();
    for (int i = 0; i < count; i++) {
        PushBack(*arr[i]);
        delete arr[i];
    }
    delete[] arr;
}

template <class T>
int DequeSegmented<T>::FindSubsequence(Sequence<T>* pattern) const {
    if (pattern->GetLength() == 0) 
        return 0;
    if (pattern->GetLength() > totalCount) 
        return -1;
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

template <class T>
Sequence<T>* DequeSegmented<T>::Merge(Sequence<T>* other, std::function<bool(T, T)> comparator) {
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

template <class T>
template <class R>
Sequence<R>* DequeSegmented<T>::Map(std::function<R(T)> func) const {
    DequeSegmented<R>* result = new DequeSegmented<R>(segmentSize);
    IEnumerator<T>* en = this->GetEnumerator();
    while (en->MoveNext()) 
        result->PushBack(func(en->Current()));
    delete en;
    return result;
}

template <class T>
Sequence<T>* DequeSegmented<T>::Where(std::function<bool(T)> func) const {
    DequeSegmented<T>* result = new DequeSegmented<T>(segmentSize);
    IEnumerator<T>* en = this->GetEnumerator();
    while (en->MoveNext()) {
        if (func(en->Current())) result->PushBack(en->Current());
    }
    delete en;
    return result;
}

template <class T>
T DequeSegmented<T>::Reduce(std::function<T(T, T)> func, T start) const {
    T result = start;
    IEnumerator<T>* en = this->GetEnumerator();
    while (en->MoveNext()) 
        result = func(result, en->Current());
    delete en;
    return result;
}

template <class T>
Option<T> DequeSegmented<T>::TryGet(int index) const {
    try {
        return Option<T>::Some(Get(index));
    } catch (...) {
        return Option<T>::None();
    }
}

template <class T>
IEnumerator<T>* DequeSegmented<T>::GetEnumerator() const {
    return new DequeEnumerator<T>(this);
}

template <class T>
int DequeSegmented<T>::GetSegmentCount() const {
    return segmentCount;
}

template <class T>
int DequeSegmented<T>::GetSegmentSize() const {
    return segmentSize;
}

template <class T>
double DequeSegmented<T>::GetMemoryEfficiency() const {
    int totalCapacity = segmentCount * segmentSize;
    if (totalCapacity == 0) 
        return 0.0;
    return (double)totalCount / totalCapacity * 100.0;
}