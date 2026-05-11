#pragma once

#include "stream_adapters.hpp"

template<class T>
CsvReadStream<T>::CsvReadStream(ReadOnlyStream<std::string>* stream, T (*parseFunc)(Sequence<std::string>*), char delim)
    : fileStream(stream), parser(parseFunc), delimiter(delim) {}

template<class T>
CsvReadStream<T>::~CsvReadStream() {}

template<class T>
void CsvReadStream<T>::Open() {
    fileStream->Open();
    this->isOpen = true;
    this->position = 0;
}

template<class T>
void CsvReadStream<T>::Close() {
    fileStream->Close();
    this->isOpen = false;
}

template<class T>
bool CsvReadStream<T>::IsEndOfStream() const {
    return fileStream->IsEndOfStream();
}

template<class T>
T CsvReadStream<T>::Read() {
    if (IsEndOfStream()) throw InvalidOperationException("End of stream");
    
    std::string line = fileStream->Read();
    Sequence<std::string>* fields = new MutableArraySequence<std::string>();
    
    std::string field;
    for (char c : line) {
        if (c == delimiter) {
            fields->Append(field);
            field.clear();
        } else {
            field += c;
        }
    }
    if (!field.empty()) fields->Append(field);
    
    T result = parser(fields);
    delete fields;
    
    this->position++;
    return result;
}

template<class T>
size_t CsvReadStream<T>::Seek(size_t index) {
    return fileStream->Seek(index);
}

template<class T>
JsonReadStream<T>::JsonReadStream(Sequence<T>* seq) : items(seq), index(0) {}

template<class T>
void JsonReadStream<T>::Open() {
    this->isOpen = true;
    index = 0;
}

template<class T>
void JsonReadStream<T>::Close() {
    this->isOpen = false;
}

template<class T>
bool JsonReadStream<T>::IsEndOfStream() const {
    return index >= (size_t)items->GetLength();
}

template<class T>
T JsonReadStream<T>::Read() {
    if (IsEndOfStream()) throw InvalidOperationException("End of stream");
    return items->Get(index++);
}

template<class T>
size_t JsonReadStream<T>::Seek(size_t newIndex) {
    if (newIndex > (size_t)items->GetLength()) throw IndexOutOfRangeException("Seek out of range");
    index = newIndex;
    return index;
}