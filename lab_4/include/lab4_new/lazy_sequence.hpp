#pragma once

#include "../lab3_base/sequence.h"
#include "cardinal.hpp"
#include "../lab3_base/option.h"
#include "../lab3_base/exceptions.h"


template<class T> class IGenerator;
template<class T> class RecursiveGenerator;

template<class T>
class LazySequence : public Sequence<T> {
private:
    IGenerator<T>* generator; 
    Sequence<T>* materialized;
    Cardinal cardinalLength;
    bool isOwner;
    
    friend class RecursiveGenerator<T>;

protected:
    virtual bool CanGenerate(size_t position) const;

public:
    LazySequence();
    LazySequence(T* items, size_t count);
    LazySequence(Sequence<T>* seq);
    LazySequence(T (*rule)(Sequence<T>*), Cardinal len = Cardinal::Infinity());
    LazySequence(const LazySequence<T>& other);
    virtual ~LazySequence();

    T GetFirst() const override;
    T GetLast() const override;
    T Get(size_t index) const override;
    LazySequence<T>* GetSubsequence(int startIndex, int endIndex) const;
    
    int GetLength() const override;
    Cardinal GetCardinalLength() const;
    size_t GetMaterializedCount() const;

    Sequence<T>* Append(const T& item) override;
    Sequence<T>* Prepend(const T& item) override;
    Sequence<T>* InsertAt(const T& item, int index) override;
    Sequence<T>* Concat(const Sequence<T>& list) override;
    Sequence<T>* InsertSequenceAt(const Sequence<T>& other, int index);

    template<class R>
    Sequence<R>* Map(std::function<R(T)> func) const;

    Sequence<T>* Where(std::function<bool(T)> func) const override;
    T Reduce(std::function<T(T, T)> func, T start) const override;

    Sequence<T>* GetMaterializedSequence() const;
    void MaterializeUpTo(size_t index) const;
    double GetMemoizationRatio() const;

    size_t GetCount() const override;
    Sequence<T>* Clone() const override;
    void Set(size_t index, const T& value) override;
    Option<T> TryGet(int index) const override;
    IEnumerator<T>* GetEnumerator() const override;
    T operator[](int index) const override;
};

#include "lazy_sequence.tpp"