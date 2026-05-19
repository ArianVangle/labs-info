#pragma once
#include <algorithm>
#include <functional>

#include "dynamic_array.h"
#include "iterators.h"
#include "option.h"
#include "sequence.h"

template <class T>
class DequeSegmented;

template <class T>
class Segment : public DynamicArray<T> {
   private:
    int count;

   public:
    Segment(int cap = 4);
    Segment(const Segment<T>& other);

    void PushBack(const T& item);
    void PushFront(const T& item);
    T PopBack();
    T PopFront();

    T Get(int index) const;
    void Set(int index, const T& value);

    int GetSize() const;
    bool IsFull() const;
    bool IsEmpty() const;
};


template <class T>
class DequeSegmented : public Sequence<T> {
   private:
    Segment<T>** segments;
    int segmentCapacity;
    int segmentCount;
    int segmentSize;
    int totalCount;
    void EnsureCapacity();
    void EnsureFrontCapacity();

   public:
    DequeSegmented(int segSize = 4);
    DequeSegmented(const DequeSegmented<T>& other);
    virtual ~DequeSegmented();

    T operator[](int index) const override;

    void PushFront(const T& item);
    void PushBack(const T& item);

    T PopFront();
    T PopBack();
    T GetFront() const;
    T GetBack() const;

    void Clear();

    T Get(size_t index) const override;
    size_t GetCount() const override;
    int GetLength() const override;
    T GetFirst() const override;
    T GetLast() const override;
    IEnumerator<T>* GetEnumerator() const override;
    int GetSegmentCount() const;
    int GetSegmentSize() const;
    double GetMemoryEfficiency() const;
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override;

    Sequence<T>* Append(const T& item) override;
    Sequence<T>* Prepend(const T& item) override;
    Sequence<T>* InsertAt(const T& item, int index) override;
    void Set(size_t index, const T& value) override;

    void Sort(std::function<bool(T, T)> comparator = [](T a, T b) { return a < b; });
    Sequence<T>* Clone() const override;

    Sequence<T>* Concat(const Sequence<T>& list) override;
    int FindSubsequence(const Sequence<T>& pattern) const;
    Sequence<T>* Merge(const Sequence<T>& other, std::function<bool(T, T)> comparator = [](T a, T b) { return a < b; });
    template <class R>
    Sequence<R>* Map(std::function<R(T)> func) const;
    Sequence<T>* Where(std::function<bool(T)> func) const override;
    T Reduce(std::function<T(T, T)> func, T start) const override;
    
    Option<T> TryGet(int index) const override;


};

#include "deque_segmented.tpp"