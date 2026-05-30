#pragma once

#include "algorithms.hpp"
#include "../lab3_base/array_sequence.h"

template<class T>
OnlineStatistics<T>::OnlineStatistics() : count(0), sum(0), hasValues(false) {
    values = new MutableArraySequence<T>();
}

template<class T>
OnlineStatistics<T>::~OnlineStatistics() {
    delete values;
}

template<class T>
void OnlineStatistics<T>::Add(T value) {
    values->Append(value);
    
    if (!hasValues) {
        minVal = maxVal = value;
        hasValues = true;
    } else {
        if (value < minVal) minVal = value;
        if (value > maxVal) maxVal = value;
    }
    
    sum += value;
    count++;
}

template<class T>
T OnlineStatistics<T>::GetMean() const {
    if (count == 0) throw InvalidOperationException("No values");
    return static_cast<T>(sum / count);
}

template<class T>
T OnlineStatistics<T>::GetMedian() {
    if (count == 0) throw InvalidOperationException("No values");
    
    Sequence<T>* sorted = values->Clone();
    for (size_t i = 0; i < count - 1; i++) {
        for (size_t j = 0; j < count - i - 1; j++) {
            if (sorted->Get(j) > sorted->Get(j + 1)) {
                T temp = sorted->Get(j);
                sorted->Set(j, sorted->Get(j + 1));
                sorted->Set(j + 1, temp);
            }
        }
    }
    
    T median = sorted->Get(count / 2);
    delete sorted;
    return median;
}

template<class T>
T OnlineStatistics<T>::GetMin() const {
    if (!hasValues) throw InvalidOperationException("No values");
    return minVal;
}

template<class T>
T OnlineStatistics<T>::GetMax() const {
    if (!hasValues) throw InvalidOperationException("No values");
    return maxVal;
}

template<class T>
size_t OnlineStatistics<T>::GetCount() const { return count; }

template<class T>
void OnlineStatistics<T>::Reset() {
    count = 0;
    sum = 0;
    hasValues = false;
    delete values;
    values = new MutableArraySequence<T>();
}

template<class T>
OnlineStatistics<T> StreamStatistics<T>::Collect(ReadOnlyStream<T>* stream) {
    OnlineStatistics<T> stats;
    stream->Open();
    
    while (!stream->IsEndOfStream()) {
        stats.Add(stream->Read());
    }
    
    stream->Close();
    return stats;
}

template<class T>
StreamSorter<T>::StreamSorter(bool (*comp)(T, T), size_t bufSize)
    : comparator(comp), bufferSize(bufSize) {}

template<class T>
ReadOnlyStream<T>* StreamSorter<T>::SortWithHeap(ReadOnlyStream<T>* input) {
    auto* output = new MutableArraySequence<T>();
    Sequence<T>* heap = new MutableArraySequence<T>();
    
    input->Open();
    
    while (!input->IsEndOfStream() && (int)heap->GetLength() < (int)bufferSize) {
        heap->Append(input->Read());
    }
    
    size_t n = heap->GetLength();
    if (n > 1) {
        int startIdx = (n / 2) - 1;
        for (int i = startIdx; i >= 0; --i) {
            size_t current = i;
            while (true) {
                size_t left = 2 * current + 1;
                size_t right = 2 * current + 2;
                size_t smallest = current;
                
                if (left < n && comparator(heap->Get(left), heap->Get(smallest))) smallest = left;
                if (right < n && comparator(heap->Get(right), heap->Get(smallest))) smallest = right;
                
                if (smallest != current) {
                    T temp = heap->Get(current);
                    heap->Set(current, heap->Get(smallest));
                    heap->Set(smallest, temp);
                    current = smallest;
                } else {
                    break;
                }
            }
        }
    }
    
    while (!input->IsEndOfStream()) {
        output->Append(heap->Get(0));
        heap->Set(0, input->Read());
        
        size_t current = 0;
        while (true) {
            size_t left = 2 * current + 1;
            size_t right = 2 * current + 2;
            size_t smallest = current;
            
            if (left < n && comparator(heap->Get(left), heap->Get(smallest))) smallest = left;
            if (right < n && comparator(heap->Get(right), heap->Get(smallest))) smallest = right;
            
            if (smallest != current) {
                T temp = heap->Get(current);
                heap->Set(current, heap->Get(smallest));
                heap->Set(smallest, temp);
                current = smallest;
            } else {
                break;
            }
        }
    }
    
    size_t heapSize = n;
    while (heapSize > 0) {
        output->Append(heap->Get(0));
        
        heap->Set(0, heap->Get(heapSize - 1));
        heapSize--;
        
        if (heapSize > 0) {
            size_t current = 0;
            while (true) {
                size_t left = 2 * current + 1;
                size_t right = 2 * current + 2;
                size_t smallest = current;
                
                if (left < heapSize && comparator(heap->Get(left), heap->Get(smallest))) smallest = left;
                if (right < heapSize && comparator(heap->Get(right), heap->Get(smallest))) smallest = right;
                
                if (smallest != current) {
                    T temp = heap->Get(current);
                    heap->Set(current, heap->Get(smallest));
                    heap->Set(smallest, temp);
                    current = smallest;
                } else {
                    break;
                }
            }
        }
    }
    
    delete heap;
    input->Close();
    return new SequenceReadStream<T>(output, true);
}

template<class T>
size_t CopyStream(ReadOnlyStream<T>* src, WriteOnlyStream<T>* dst) {
    size_t count = 0;
    while (!src->IsEndOfStream()) {
        dst->Write(src->Read());
        count++;
    }
    return count;
}