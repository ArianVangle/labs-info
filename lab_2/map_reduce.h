#pragma once

#include "array_sequence.h"
#include "option.h"
#include "sequence.h"

template <class T1, class T2>
struct Pair {
    T1 first;
    T2 second;

    Pair() : first(T1()), second(T2()) {}
    Pair(T1 f, T2 s) : first(f), second(s) {}
};

template <class T1, class T2>
struct Tuple2 {
    T1 item1;
    T2 item2;

    Tuple2() : item1(T1()), item2(T2()) {}
    Tuple2(T1 i1, T2 i2) : item1(i1), item2(i2) {}
};

template <class T1, class T2>
Sequence<Tuple2<T1, T2>>* Zip(Sequence<T1>* seq1, Sequence<T2>* seq2) {
    int minLen = (seq1->GetLength() < seq2->GetLength()) ? seq1->GetLength()
                                                         : seq2->GetLength();

    auto* result = new MutableArraySequence<Tuple2<T1, T2>>();

    for (int i = 0; i < minLen; i++) {
        result->Append(Tuple2<T1, T2>(seq1->Get(i), seq2->Get(i)));
    }

    return result;
}

template <class T1, class T2>
Pair<Sequence<T1>*, Sequence<T2>*> Unzip(Sequence<Tuple2<T1, T2>>* seq) {
    auto* result1 = new MutableArraySequence<T1>();
    auto* result2 = new MutableArraySequence<T2>();

    for (int i = 0; i < seq->GetLength(); i++) {
        result1->Append(seq->Get(i).item1);
        result2->Append(seq->Get(i).item2);
    }

    return Pair<Sequence<T1>*, Sequence<T2>*>(result1, result2);
}

template <class T, class Func>
Sequence<Sequence<T>*>* Split(Sequence<T>* seq, Func predicate) {
    auto* result = new MutableArraySequence<Sequence<T>*>();
    auto* currentChunk = new MutableArraySequence<T>();

    for (int i = 0; i < seq->GetLength(); i++) {
        T val = seq->Get(i);
        if (predicate(val)) {
            result->Append(currentChunk);
            currentChunk = new MutableArraySequence<T>();
        } else {
            currentChunk->Append(val);
        }
    }
    result->Append(currentChunk);

    return result;
}

template <class T>
Sequence<T>* Slice(Sequence<T>* seq, int index, int count,
                   Sequence<T>* s = nullptr) {
    int len = seq->GetLength();

    if (index < 0) {
        index = len + index;
    }

    if (index < 0 || index >= len) {
        throw IndexOutOfRangeException("Slice: invalid index");
    }

    if (count < 0) {
        throw IndexOutOfRangeException("Slice: invalid count");
    }

    auto* result = new MutableArraySequence<T>();

    for (int i = 0; i < index; i++) {
        result->Append(seq->Get(i));
    }

    if (s != nullptr) {
        for (int i = 0; i < s->GetLength(); i++) {
            result->Append(s->Get(i));
        }
    }

    int skipEnd = index + count;
    for (int i = skipEnd; i < len; i++) {
        result->Append(seq->Get(i));
    }

    return result;
}

template <class T>
Sequence<T>* From(T* arr, int count) {
    return new MutableArraySequence<T>(arr, count);
}

template <class T>
Sequence<T>* From(std::initializer_list<T> list) {
    T* arr = new T[list.size()];
    int i = 0;
    for (auto& item : list) {
        arr[i++] = item;
    }
    Sequence<T>* seq = new MutableArraySequence<T>(arr, list.size());
    delete[] arr;
    return seq;
}

template <class T>
Sequence<T>* Concat(Sequence<T>* seq1, Sequence<T>* seq2) {
    auto* result = new MutableArraySequence<T>();

    for (int i = 0; i < seq1->GetLength(); i++) {
        result->Append(seq1->Get(i));
    }

    for (int i = 0; i < seq2->GetLength(); i++) {
        result->Append(seq2->Get(i));
    }

    return result;
}

template <class T, class Func>
Option<T> Find(Sequence<T>* seq, Func predicate) {
    for (int i = 0; i < seq->GetLength(); i++) {
        T val = seq->Get(i);
        if (predicate(val)) {
            return Option<T>::Some(val);
        }
    }
    return Option<T>::None();
}

template <class T>
Option<T> First(Sequence<T>* seq) {
    if (seq->GetLength() == 0) {
        return Option<T>::None();
    }
    return Option<T>::Some(seq->GetFirst());
}

template <class T, class Func>
bool Any(Sequence<T>* seq, Func predicate) {
    for (int i = 0; i < seq->GetLength(); i++) {
        if (predicate(seq->Get(i))) {
            return true;
        }
    }
    return false;
}

template <class T, class Func>
bool All(Sequence<T>* seq, Func predicate) {
    for (int i = 0; i < seq->GetLength(); i++) {
        if (!predicate(seq->Get(i))) {
            return false;
        }
    }
    return true;
}