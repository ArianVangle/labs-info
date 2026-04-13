#pragma once
#include "array_sequence.h"
#include "iterators.h"
#include "option.h"
#include "sequence.h"

template <class T1, class T2>
struct Tuple2 {
    T1 item1;
    T2 item2;

    Tuple2() : item1(T1()), item2(T2()) {
    }
    Tuple2(const T1& i1, const T2& i2) : item1(i1), item2(i2) {
    }
    
    bool operator==(const Tuple2<T1, T2>& other) const {
        return item1 == other.item1 && item2 == other.item2;
    }
    bool operator!=(const Tuple2<T1, T2>& other) const {
        return !(*this == other);
    }
};

template <class T1, class T2>
Sequence<Tuple2<T1, T2>>* Zip(const Sequence<T1>& seq1, const Sequence<T2>& seq2);

template <class T1, class T2>
Tuple2<Sequence<T1>*, Sequence<T2>*> Unzip(const Sequence<Tuple2<T1, T2>>& seq);

template <class T, class Func>
Sequence<Sequence<T>*>* Split(const Sequence<T>& seq, Func predicate);

template <class T>
Sequence<T>* Slice(const Sequence<T>& seq, int index, int count, Sequence<T>* s = nullptr);

template <class T>
Sequence<T>* From(T& arr, int count);

template <class T>
Sequence<T>* From(const std::initializer_list<T>& list);

template <class T>
Sequence<T>* Concat(const Sequence<T>& seq1, const Sequence<T>& seq2);

template <class T, class Func>
Option<T> Find(const Sequence<T>& seq, Func predicate);

template <class T>
Option<T> First(const Sequence<T>& seq);

template <class T, class Func>
bool Any(const Sequence<T>& seq, Func predicate);

template <class T, class Func>
bool All(const Sequence<T>& seq, Func predicate);

#include "map_reduce.tpp"