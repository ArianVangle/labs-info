#pragma once

#include "stream.hpp"
#include "lazy_sequence.hpp"
#include "read_only_stream.hpp"

template<class T>
class OnlineStatistics {
private:
    size_t count;
    double sum;
    T minVal;
    T maxVal;
    bool hasValues;
    Sequence<T>* values;
    
public:
    OnlineStatistics();
    ~OnlineStatistics();
    
    void Add(T value);
    T GetMean() const;
    T GetMedian();
    T GetMin() const;
    T GetMax() const;
    size_t GetCount() const;
    void Reset();
};

template<class T>
class StreamStatistics {
public:
    static OnlineStatistics<T> Collect(ReadOnlyStream<T>* stream);
};

template<class T>
class StreamSorter {
private:
    bool (*comparator)(T, T);
    size_t bufferSize;
    
public:
    StreamSorter(bool (*comp)(T, T), size_t bufSize = 100);
    ReadOnlyStream<T>* SortWithHeap(ReadOnlyStream<T>* input);
};

#include "algorithms.tpp"