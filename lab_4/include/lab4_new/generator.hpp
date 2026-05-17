#pragma once
#include "../lab3_base/sequence.h"
#include "../lab3_base/option.h"
#include "../lab3_base/exceptions.h"
#include "../lab3_base/iterators.h"
#include "cardinal.hpp"
#include "circular_buffer.hpp"

template<class T> class LazySequence;

template<class T>
class IGenerator {
public:
    virtual ~IGenerator() = default;
    virtual T GetNext() = 0;
    virtual bool HasNext() const = 0;
    virtual void Reset() = 0;
    virtual size_t GetPosition() const = 0;
};

template<class T>
class RecursiveGenerator : public IGenerator<T>, public IEnumerator<T> {
private:
    LazySequence<T>* owner;
    T (*generationRule)(Sequence<T>*);
    CircularBuffer<T>* cache;
    size_t position;
public:
    RecursiveGenerator(LazySequence<T>* seq, T (*rule)(Sequence<T>*), size_t maxSize = 100);
    ~RecursiveGenerator();
    
    T GetNext() override;
    bool HasNext() const override;
    void Reset() override;
    size_t GetPosition() const override { return position; }

    T Current() const override;
    bool MoveNext() override;
};

template<class T>
class ConcatGenerator : public IGenerator<T> {
private:
    IGenerator<T>* gen1;
    const Sequence<T>* seq2;
    IEnumerator<T>* iter2;
    bool switched;
public:
    ConcatGenerator(IGenerator<T>* g1, const Sequence<T>* s2)
    : gen1(g1), seq2(s2), iter2(nullptr), switched(false) {}
    
    ~ConcatGenerator() {
        delete iter2;
    }
    
    T GetNext() override {
        if (!switched) {
            if (gen1->HasNext()) {
                return gen1->GetNext();
            } else {
                switched = true;
                iter2 = seq2->GetEnumerator();
            }
        }
        if (switched && iter2) {
            if (iter2->MoveNext()) {
                return iter2->Current();
            }
        }
        throw InvalidOperationException("End of concatenated sequence");
    }
    
    bool HasNext() const override {
        if (!switched) {
            return gen1->HasNext();
        }
        return true; 
    }
    
    void Reset() override {
        switched = false;
        delete iter2;
        iter2 = nullptr;
        gen1->Reset();
    }
    
    size_t GetPosition() const override {
        return gen1->GetPosition();
    }
};

#include "generator.tpp"