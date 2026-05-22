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

template<class T>
class InsertAtGenerator : public IGenerator<T> {
private:
    IGenerator<T>* genSource;
    const Sequence<T>* seqToInsert;
    IEnumerator<T>* iterInsert;
    int splitIndex;
    int currentPos;
    enum State { PREFIX, INSERTING, SUFFIX } state;
    bool insertExhausted;

public:
    InsertAtGenerator(IGenerator<T>* src, const Sequence<T>* ins, int idx)
        : genSource(src), seqToInsert(ins), iterInsert(nullptr), 
          splitIndex(idx), currentPos(0), state(PREFIX), insertExhausted(false) {}

    ~InsertAtGenerator() { delete iterInsert; }

    T GetNext() override {
        switch (state) {
            case PREFIX:
                if (currentPos < splitIndex && genSource->HasNext()) {
                    currentPos++;
                    return genSource->GetNext();
                }
                state = INSERTING;
                iterInsert = seqToInsert->GetEnumerator();
                [[fallthrough]];
                
            case INSERTING:
                if (iterInsert && iterInsert->MoveNext()) {
                    return iterInsert->Current();
                }
                insertExhausted = true;
                state = SUFFIX;
                [[fallthrough]];
                
            case SUFFIX:
                if (genSource->HasNext()) {
                    currentPos++;
                    return genSource->GetNext();
                }
                throw InvalidOperationException("End of inserted sequence");
        }
        return T();
    }

    bool HasNext() const override {
        if (state == PREFIX) return currentPos < splitIndex && genSource->HasNext();
        if (state == INSERTING) return true;
        return genSource->HasNext();
    }

    void Reset() override {
        state = PREFIX;
        currentPos = 0;
        insertExhausted = false;
        delete iterInsert;
        iterInsert = nullptr;
        genSource->Reset();
    }

    size_t GetPosition() const override { return currentPos; }
};


template<class T, class R>
class MapGenerator : public IGenerator<R> {
    IGenerator<T>* source;
    std::function<R(T)> func;
public:
    MapGenerator(IGenerator<T>* src, std::function<R(T)> f) : source(src), func(f) {}
    ~MapGenerator() { delete source; }

    R GetNext() override { return func(source->GetNext()); }
    bool HasNext() const override { return source->HasNext(); }
    void Reset() override { source->Reset(); }
    size_t GetPosition() const override { return source->GetPosition(); }
};

template<class T>
class WhereGenerator : public IGenerator<T> {
    IGenerator<T>* source;
    std::function<bool(T)> predicate;
    T bufferedValue;
    bool hasBuffered;
public:
    WhereGenerator(IGenerator<T>* src, std::function<bool(T)> pred) 
        : source(src), predicate(pred), hasBuffered(false) {}
    ~WhereGenerator() { delete source; }
    
    T GetNext() override {
        if (hasBuffered) { hasBuffered = false; return bufferedValue; }
        while (source->HasNext()) {
            T val = source->GetNext();
            if (predicate(val)) return val;
        }
        throw InvalidOperationException("No matching element");
    }
    bool HasNext() const override {
        if (hasBuffered) return true;
        WhereGenerator<T>* self = const_cast<WhereGenerator<T>*>(this);
        try { self->bufferedValue = self->GetNext(); self->hasBuffered = true; return true; }
        catch (...) { return false; }
    }
    void Reset() override { source->Reset(); hasBuffered = false; }
    size_t GetPosition() const override { return source->GetPosition(); }
};


#include "generator.tpp"