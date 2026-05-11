#pragma once

#include "../lab3_base/sequence.h"
#include "../lab3_base/option.h"
#include "../lab3_base/exceptions.h"
#include "cardinal.hpp"
#include "../lab3_base/iterators.h"
#include "circular_buffer.hpp"

template<class T>
class LazySequence;

template<class T>
class Generator : public IEnumerator<T> {
private:
    LazySequence<T>* owner;
    T (*generationRule)(Sequence<T>*);
    CircularBuffer<T>* cache;
    size_t position;

public:
    Generator(LazySequence<T>* seq, T (*rule)(Sequence<T>*), size_t maxSize = 100);
    ~Generator();
    
    Generator(const Generator<T>&) = delete;
    Generator& operator=(const Generator<T>&) = delete;
    
    T Current() const override;
    bool MoveNext() override;
    void Reset() override;
    
    T GetNext();
    bool HasNext() const;
    Option<T> TryGetNext();
    
    size_t GetPosition() const;
    size_t GetCacheSize() const;
};

#include "generator.tpp"