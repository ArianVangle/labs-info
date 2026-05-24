#pragma once
#include "../lab3_base/sequence.h"
#include "../lab3_base/option.h"
#include "../lab3_base/exceptions.h"
#include "../lab3_base/iterators.h"
#include "cardinal.hpp"
#include "circular_buffer.hpp"
#include <functional>

template<class T> class LazySequence;

template<class T>
class IGenerator {
public:
    virtual ~IGenerator() = default;
    virtual T GetNext() = 0;
    virtual bool HasNext() const = 0;
    virtual void Reset() = 0;
    virtual size_t GetPosition() const = 0;
    virtual bool IsSelfMaterializing() const { return false; }
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
    bool IsSelfMaterializing() const override { return true; }
};

template<class T>
class ConcatGenerator : public IGenerator<T> {
private:
    const Sequence<T>* seq1;
    const Sequence<T>* seq2;
    size_t idx;
public:
    ConcatGenerator(const Sequence<T>* s1, const Sequence<T>* s2)
        : seq1(s1), seq2(s2), idx(0) {}
    ~ConcatGenerator() {}

    T GetNext() override {
        int len1 = seq1->GetLength();
        if (len1 == -1 || idx < (size_t)len1) {
            return seq1->Get(idx++);
        }
        if (seq2->GetLength() == -1 || idx - len1 < (size_t)seq2->GetLength()) {
            return seq2->Get(idx++ - len1);
        }
        throw InvalidOperationException("End of concatenated sequence");
    }

    bool HasNext() const override {
        int len1 = seq1->GetLength();
        if (len1 == -1) return true;
        if (idx < (size_t)len1) return true;
        int len2 = seq2->GetLength();
        if (len2 == -1) return true;
        return idx - len1 < (size_t)len2;
    }

    void Reset() override { idx = 0; }
    size_t GetPosition() const override { return idx; }
};

template<class T>
class InsertAtGenerator : public IGenerator<T> {
private:
    IGenerator<T>* genSource;
    Sequence<T>* seqToInsert;
    IEnumerator<T>* iterInsert;
    int splitIndex;
    int currentPos;
    enum State { PREFIX, INSERTING, SUFFIX } state;
    bool insertExhausted;
public:
    InsertAtGenerator(IGenerator<T>* src, Sequence<T>* ins, int idx)
        : genSource(src), seqToInsert(ins), iterInsert(nullptr),
          splitIndex(idx), currentPos(0), state(PREFIX), insertExhausted(false) {}
    ~InsertAtGenerator() { delete iterInsert; delete seqToInsert; }

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
        state = PREFIX; currentPos = 0; insertExhausted = false;
        delete iterInsert; iterInsert = nullptr; genSource->Reset();
    }
    size_t GetPosition() const override { return currentPos; }
};

template<class T, class R>
class MapGenerator : public IGenerator<R> {
    IGenerator<T>* source;
    std::function<R(T)> func;
public:
    MapGenerator(IGenerator<T>* src, std::function<R(T)> f) : source(src), func(f) {
        if (source) source->Reset();
    }
    ~MapGenerator() {}
    R GetNext() override { return func(source->GetNext()); }
    bool HasNext() const override { return source->HasNext(); }
    void Reset() override { source->Reset(); }
    size_t GetPosition() const override { return source->GetPosition(); }
};

template<class T>
class WhereGenerator : public IGenerator<T> {
    IGenerator<T>* source;
    std::function<bool(T)> predicate;
    
    struct CacheState {
        T buffer;
        bool hasBuffer;
        CacheState() : buffer(T()), hasBuffer(false) {}
    };
    CacheState* cache;

public:
    WhereGenerator(IGenerator<T>* src, std::function<bool(T)> pred)
        : source(src), predicate(pred), cache(new CacheState()) {
            if (source) source->Reset();
    }
    
    ~WhereGenerator() override { delete cache; }

    T GetNext() override {
        if (!HasNext()) throw InvalidOperationException("No matching element");
        T result = cache->buffer;
        cache->hasBuffer = false;
        return result;
    }

    bool HasNext() const override {
        if (cache->hasBuffer) return true;
        while (source->HasNext()) {
            T val = source->GetNext();
            if (predicate(val)) {
                cache->buffer = val;
                cache->hasBuffer = true;
                return true;
            }
        }
        return false;
    }

    void Reset() override {
        source->Reset();
        cache->hasBuffer = false;
    }

    size_t GetPosition() const override { return source->GetPosition(); }
};

#include "generator.tpp"