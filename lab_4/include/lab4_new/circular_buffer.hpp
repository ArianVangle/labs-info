#pragma once

#include "../lab3_base/exceptions.h"

template<class T>
class CircularBuffer {
private:
    T* buffer;
    size_t bufferSize;
    size_t head;
    size_t count;
    bool isFull;

public:
    CircularBuffer(size_t size) 
        : bufferSize(size), head(0), count(0), isFull(false) {
        if (size == 0) throw InvalidOperationException("Buffer size must be > 0");
        buffer = new T[size];
    }

    ~CircularBuffer() {
        delete[] buffer;
    }

    CircularBuffer(const CircularBuffer&) = delete;
    CircularBuffer& operator=(const CircularBuffer&) = delete;

    void Append(const T& value) {
        buffer[head] = value;
        head = (head + 1) % bufferSize;
        
        if (!isFull) {
            count++;
            if (count == bufferSize) {
                isFull = true;
            }
        }
    }

    T Get(size_t index) const {
        if (index >= GetLength()) {
            throw IndexOutOfRangeException("Index out of range in CircularBuffer");
        }
        
        if (isFull) {
            size_t actualIndex = (head + index) % bufferSize;
            return buffer[actualIndex];
        } else {
            return buffer[index];
        }
    }

    size_t GetLength() const {
        return isFull ? bufferSize : count;
    }

    void Clear() {
        head = 0;
        count = 0;
        isFull = false;
    }

    bool IsEmpty() const {
        return count == 0 && !isFull;
    }
    
    size_t GetCapacity() const {
        return bufferSize;
    }
};