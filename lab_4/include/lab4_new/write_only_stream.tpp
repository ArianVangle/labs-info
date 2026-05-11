#pragma once

#include "write_only_stream.hpp"

template<class T>
SequenceWriteStream<T>::SequenceWriteStream(Sequence<T>* seq) 
    : WriteOnlyStream<T>(), target(seq) {}

template<class T>
void SequenceWriteStream<T>::Open() {
    this->isOpen = true;
    this->position = 0;
}

template<class T>
void SequenceWriteStream<T>::Close() {
    this->isOpen = false;
}

template<class T>
bool SequenceWriteStream<T>::IsEndOfStream() const {
    return false;
}

template<class T>
size_t SequenceWriteStream<T>::Write(T item) {
    if (!this->isOpen) throw InvalidOperationException("Stream not open");
    target->Append(item);
    this->position++;
    return this->position;
}

template<class T>
bool SequenceWriteStream<T>::IsCanSeek() const { return false; }

template<class T>
bool SequenceWriteStream<T>::IsCanGoBack() const { return false; }

template<class T>
FileWriteStream<T>::FileWriteStream(const std::string& fname, std::string (*ser)(T))
    : WriteOnlyStream<T>(), filename(fname), serializer(ser) {}

template<class T>
FileWriteStream<T>::~FileWriteStream() {
    if (file.is_open()) file.close();
}

template<class T>
void FileWriteStream<T>::Open() {
    file.open(filename, std::ios::out | std::ios::trunc);
    if (!file.is_open()) throw InvalidOperationException("Cannot open file: " + filename);
    this->isOpen = true;
    this->position = 0;
}

template<class T>
void FileWriteStream<T>::Close() {
    if (file.is_open()) file.close();
    this->isOpen = false;
}

template<class T>
size_t FileWriteStream<T>::Write(T item) {
    if (!this->isOpen) throw InvalidOperationException("Stream not open");
    file << serializer(item) << std::endl;
    this->position++;
    return this->position;
}

template<class T>
bool FileWriteStream<T>::IsCanSeek() const { return false; }

template<class T>
bool FileWriteStream<T>::IsCanGoBack() const { return false; }