#pragma once
#include "exceptions.h"
template <class T>
class DynamicArray {
   private:
    T* items;
    int size;

   public:
    DynamicArray(T* items, int count);
    DynamicArray(int size);
    DynamicArray(const DynamicArray<T>& dynamicArray);
    ~DynamicArray();

    T Get(int index) const;
    int GetSize() const;
    T* GetItems() const;

    void Set(int index, const T& value);
    void Resize(int newSize);
};

#include "dynamic_array.tpp"