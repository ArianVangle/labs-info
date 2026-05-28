#pragma once
#include "dynamic_array.h"
#include <algorithm>

template <class T>
void DynamicArray<T>::Reallocate(int newCapacity) {
    if (newCapacity <= capacity) return;
    T* newItems = new T[newCapacity];
    int copyCount = std::min(size, newCapacity);
    for (int i = 0; i < copyCount; i++) 
        newItems[i] = items[i];
    delete[] items;

    items = newItems;
    capacity = newCapacity;
}

template <class T>
DynamicArray<T>::DynamicArray(T* items, int count) {
    if (count < 0) 
        throw IndexOutOfRangeException("Count cannot be negative");

    size = count;
    capacity = std::max(1, count);

    this->items = new T[capacity];
    for (int i = 0; i < size; i++) 
        this->items[i] = items[i];
}

template <class T>
DynamicArray<T>::DynamicArray(int size) {
    if (size < 0) 
        throw IndexOutOfRangeException("Size cannot be negative");
    this->size = size;
    capacity = std::max(1, size);
    items = new T[capacity];
}

template <class T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& dynamicArray) {
    size = dynamicArray.size;
    capacity = dynamicArray.capacity;
    items = new T[capacity];
    for (int i = 0; i < size; i++) 
        items[i] = dynamicArray.items[i];
}

template <class T>
DynamicArray<T>::~DynamicArray() {
    delete[] items;
}

template <class T>
T DynamicArray<T>::Get(int index) const {
    if (index < 0 || index >= size)
        throw IndexOutOfRangeException("Index out of range");
    return items[index];
}

template <class T>
int DynamicArray<T>::GetSize() const {
    return size;
}

template <class T>
void DynamicArray<T>::Set(int index, const T& value) {
    if (index < 0 || index >= size)
        throw IndexOutOfRangeException("Index out of range");
    items[index] = value;
}

template <class T>
void DynamicArray<T>::Resize(int newSize) {
    if (newSize < 0) throw IndexOutOfRangeException("New size cannot be negative");
    
    if (newSize <= capacity) {
        size = newSize;
        return;
    }
    
    int newCapacity = std::max(newSize, capacity * 2);
    Reallocate(newCapacity);
    size = newSize;
}

template <class T>
T* DynamicArray<T>::GetItems() const {
    return items;
}