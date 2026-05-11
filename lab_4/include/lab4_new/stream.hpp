#pragma once

#include "../lab3_base/option.h"
#include "../lab3_base/exceptions.h"
#include <string>


template<class T> class Sequence;
template<class T> class LazySequence;
template<class T> class Stream;

template<class T>
class Stream {
protected:
    bool isOpen;
    size_t position;
public:
    Stream() : isOpen(false), position(0) {}
    virtual ~Stream() {}
    virtual void Open() = 0;
    virtual void Close() = 0;
    virtual bool IsEndOfStream() const = 0;
    virtual size_t GetPosition() const { return position; }
    virtual bool IsCanSeek() const { return false; }
    virtual bool IsCanGoBack() const { return false; }
    bool IsOpen() const { return isOpen; }
};

template<class T>
class ReadOnlyStream : public Stream<T> {
public:
    ReadOnlyStream() : Stream<T>() {}
    virtual ~ReadOnlyStream() {}
    virtual T Read() = 0;
    virtual Option<T> TryRead() {
        try { return Option<T>::Some(Read()); }
        catch (...) { return Option<T>::None(); }
    }
    virtual size_t Seek(size_t) { 
    throw InvalidOperationException("Seek not supported"); 
    }
};

template<class T>
class WriteOnlyStream : public Stream<T> {
public:
    WriteOnlyStream() : Stream<T>() {}
    virtual ~WriteOnlyStream() {}
    virtual size_t Write(T item) = 0;
};

#include "stream.tpp"