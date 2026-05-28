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
        T value = input->Read();
        heap->Append(value);
        
        for (size_t i = heap->GetLength() - 1; i > 0; i--) {
            size_t parent = (i - 1) / 2;
            if (comparator(heap->Get(i), heap->Get(parent))) {
                T temp = heap->Get(i);
                heap->Set(i, heap->Get(parent));
                heap->Set(parent, temp);
            }
        }
    }
    
    while (!input->IsEndOfStream()) {
        T smallest = heap->Get(0);
        output->Append(smallest);
        
        T newValue = input->Read();
        heap->Set(0, newValue);
        
        size_t i = 0;
        while (2 * i + 1 < (size_t)heap->GetLength()) {
            size_t left = 2 * i + 1;
            size_t right = 2 * i + 2;
            size_t smallest = left;
            
            if (right < (size_t)heap->GetLength() && comparator(heap->Get(right), heap->Get(left))) {
                smallest = right;
            }
            
            if (comparator(heap->Get(smallest), heap->Get(i))) {
                T temp = heap->Get(i);
                heap->Set(i, heap->Get(smallest));
                heap->Set(smallest, temp);
                i = smallest;
            } else {
                break;
            }
        }
    }
    
    while (heap->GetLength() > 0) {
        T smallest = heap->Get(0);
        output->Append(smallest);
        
        if (heap->GetLength() == 1) {
            delete heap;
            heap = new MutableArraySequence<T>();
            break;
        }
        
        T lastValue = heap->Get(heap->GetLength() - 1);
        heap->Set(0, lastValue);
        
        size_t i = 0;
        while (2 * i + 1 < (size_t)heap->GetLength() - 1) {
            size_t left = 2 * i + 1;
            size_t right = 2 * i + 2;
            size_t smallest = left;
            
            if (right < (size_t)heap->GetLength() - 1 && comparator(heap->Get(right), heap->Get(left))) {
                smallest = right;
            }
            
            if (comparator(heap->Get(smallest), heap->Get(i))) {
                T temp = heap->Get(i);
                heap->Set(i, heap->Get(smallest));
                heap->Set(smallest, temp);
                i = smallest;
            } else {
                break;
            }
        }
        
        Sequence<T>* newHeap = new MutableArraySequence<T>();
        for (size_t j = 0; j < (size_t)heap->GetLength() - 1; j++) {
            newHeap->Append(heap->Get(j));
        }
        delete heap;
        heap = newHeap;
    }
    
    delete heap;
    input->Close();
    return new SequenceReadStream<T>(output, true);
}