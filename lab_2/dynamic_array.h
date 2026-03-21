#pragma once
#include <iostream>

#include "exceptions.h"

template <class T>
class DynamicArray {
   private:
    T* items;
    int size;

   public:
    DynamicArray(T* items, int count) {
        if (count < 0)
            throw IndexOutOfRangeException("Count cannot be negative");
        size = count;
        this->items = new T[size];
        for (int i = 0; i < size; i++) {
            this->items[i] = items[i];
        }
    }

    DynamicArray(int size) {
        if (size < 0) 
            throw IndexOutOfRangeException("Size cannot be negative");
        this->size = size;
        items = new T[size];
    }

    DynamicArray(const DynamicArray<T>& dynamicArray) {
        size = dynamicArray.size;
        items = new T[size];
        for (int i = 0; i < size; i++) {
            items[i] = dynamicArray.items[i];
        }
    }

    ~DynamicArray() { delete[] items; }

    T Get(int index) const {
        if (index < 0 || index >= size)
            throw IndexOutOfRangeException("Index out of range");
        return items[index];
    }

    int GetSize() const { return size; }

    void Set(int index, T value) {
        if (index < 0 || index >= size)
            throw IndexOutOfRangeException("Index out of range");
        items[index] = value;
    }

    void Resize(int newSize) {
        if (newSize < 0)
            throw IndexOutOfRangeException("New size cannot be negative");
        T* newItems = new T[newSize];
        int copyCount = (newSize < size) ? newSize : size;
        for (int i = 0; i < copyCount; i++) {
            newItems[i] = items[i];
        }
        delete[] items;
        items = newItems;
        size = newSize;
    }

    T* GetItems() const { return items; }
};