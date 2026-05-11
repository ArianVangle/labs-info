#pragma once

#include <fstream>
#include <sstream>

#include "read_only_stream.hpp"

template<class T>
SequenceReadStream<T>::SequenceReadStream(Sequence<T>* seq) 
    : source(seq), sourceLength(seq->GetLength()) {}

template<class T>
void SequenceReadStream<T>::Open() {
    this->isOpen = true;
    this->position = 0;
}

template<class T>
void SequenceReadStream<T>::Close() {
    this->isOpen = false;
}

template<class T>
bool SequenceReadStream<T>::IsEndOfStream() const {
    return this->position >= sourceLength;
}

template<class T>
T SequenceReadStream<T>::Read() {
    if (!this->isOpen) throw InvalidOperationException("Stream not open");
    if (IsEndOfStream()) throw InvalidOperationException("End of stream");
    
    T value = source->Get(this->position);
    this->position++;
    return value;
}

template<class T>
bool SequenceReadStream<T>::IsCanSeek() const { return true; }
template<class T>
bool SequenceReadStream<T>::IsCanGoBack() const { return true; }

template<class T>
size_t SequenceReadStream<T>::Seek(size_t index) {
    if (index > sourceLength) throw IndexOutOfRangeException("Seek out of range");
    this->position = index;
    return this->position;
}

template<class T>
LazySequenceReadStream<T>::LazySequenceReadStream(LazySequence<T>* seq) : source(seq) {}

template<class T>
void LazySequenceReadStream<T>::Open() {
    this->isOpen = true;
    this->position = 0;
}

template<class T>
void LazySequenceReadStream<T>::Close() {
    this->isOpen = false;
}

template<class T>
bool LazySequenceReadStream<T>::IsEndOfStream() const {
    Cardinal len = source->GetCardinalLength();
    return len.IsFinite() && this->position >= len.GetValue();
}

template<class T>
T LazySequenceReadStream<T>::Read() {
    if (!this->isOpen) throw InvalidOperationException("Stream not open");
    if (IsEndOfStream()) throw InvalidOperationException("End of stream");
    
    T value = source->Get(this->position);
    this->position++;
    return value;
}

template<class T>
bool LazySequenceReadStream<T>::IsCanSeek() const { return true; }
template<class T>
bool LazySequenceReadStream<T>::IsCanGoBack() const { return true; }

template<class T>
size_t LazySequenceReadStream<T>::Seek(size_t index) {
    Cardinal len = source->GetCardinalLength();
    if (len.IsFinite() && index >= len.GetValue()) {
        throw IndexOutOfRangeException("Seek out of range");
    }
    this->position = index;
    return this->position;
}

inline FileStream::FileStream(const std::string& fname, std::string (*deser)(const std::string&))
    : filename(fname), deserializer(deser) {}

inline FileStream::~FileStream() {
    if (file.is_open()) file.close();
}

inline void FileStream::Open() {
    file.open(filename);
    if (!file.is_open()) throw InvalidOperationException("Cannot open file: " + filename);
    this->isOpen = true;
    this->position = 0;
}

inline void FileStream::Close() {
    if (file.is_open()) file.close();
    this->isOpen = false;
}

inline bool FileStream::IsEndOfStream() const {
    return file.eof();
}

inline std::string FileStream::Read() {
    if (!this->isOpen) throw InvalidOperationException("Stream not open");
    if (IsEndOfStream()) throw InvalidOperationException("End of stream");
    
    std::string line;
    std::getline(file, line);
    this->position++;
    return deserializer(line);
}

inline bool FileStream::IsCanSeek() const { return true; }
inline bool FileStream::IsCanGoBack() const { return false; }

inline size_t FileStream::Seek(size_t index) {
    if (!this->isOpen) throw InvalidOperationException("Stream not open");
    file.clear();
    file.seekg(0, std::ios::beg);
    for (size_t i = 0; i < index; i++) {
        std::string dummy;
        std::getline(file, dummy);
    }
    this->position = index;
    return this->position;
}