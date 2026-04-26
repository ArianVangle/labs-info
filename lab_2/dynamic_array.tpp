#pragma once
#include "dynamic_array.h"

template <class T>
DynamicArray<T>::DynamicArray(T* items, int count) {
    if (count < 0) 
        throw IndexOutOfRangeException("Count cannot be negative");
    size = count;
    this->items = new T[size];
    for (int i = 0; i < size; i++) 
        this->items[i] = items[i];
}

template <class T>
DynamicArray<T>::DynamicArray(int size) {
    if (size < 0) 
        throw IndexOutOfRangeException("Size cannot be negative");
    this->size = size;
    items = new T[size];
}

template <class T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& dynamicArray) {
    size = dynamicArray.size;
    items = new T[size];
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
    if (newSize < 0) 
        throw IndexOutOfRangeException("New size cannot be negative");
    T* newItems = new T[newSize];
    int copyCount = (newSize < size) ? newSize : size;
    for (int i = 0; i < copyCount; i++) 
        newItems[i] = items[i];
    delete[] items;
    items = newItems;
    size = newSize;
}

template <class T>
T* DynamicArray<T>::GetItems() const {
    return items;
}