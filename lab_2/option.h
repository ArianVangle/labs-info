#pragma once
#include "exceptions.h"
template <class T>
class Option {
   private:
    bool hasValue;
    T value;

   public:
    Option();

    static Option<T> Some(const T& val);
    static Option<T> None();
    bool IsSome() const;
    bool IsNone() const;
    
    T GetValue() const;
};
#include "option.tpp"
