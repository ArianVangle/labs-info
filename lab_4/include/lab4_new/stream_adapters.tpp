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
    if (line.empty()) throw InvalidOperationException("End of stream");
    
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
JsonReadStream<T>::JsonReadStream(const std::string& fname) 
    : filename(fname), cachedItems(nullptr), index(0) {}

template<class T>
JsonReadStream<T>::~JsonReadStream() {
    if (file.is_open()) file.close();
    delete cachedItems;
}

template<class T>
void JsonReadStream<T>::Open() {
    if (cachedItems == nullptr) {
        file.open(filename);
        if (!file.is_open()) throw InvalidOperationException("Cannot open file: " + filename);
        
        std::string content((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        file.close();

        content.erase(std::remove(content.begin(), content.end(), '['), content.end());
        content.erase(std::remove(content.begin(), content.end(), ']'), content.end());
        content.erase(std::remove(content.begin(), content.end(), ' '), content.end());
        
        cachedItems = new MutableArraySequence<T>();
        std::stringstream ss(content);
        std::string item;
        while (std::getline(ss, item, ',')) {
            if (!item.empty()) {
                std::istringstream converter(item);
                T val;
                converter >> val;
                cachedItems->Append(val);
            }
        }
    }
    
    this->isOpen = true;
    index = 0;
}

template<class T>
void JsonReadStream<T>::Close() {
    this->isOpen = false;
}

template<class T>
bool JsonReadStream<T>::IsEndOfStream() const {
    return index >= (size_t)cachedItems->GetLength();
}

template<class T>
T JsonReadStream<T>::Read() {
    if (!this->isOpen) throw InvalidOperationException("Stream not open");
    if (IsEndOfStream()) throw InvalidOperationException("End of stream");
    
    T val = cachedItems->Get(index++);
    this->position = index;
    return val;
}

template<class T>
size_t JsonReadStream<T>::Seek(size_t newIndex) {
    if (!this->isOpen) throw InvalidOperationException("Stream not open");
    if (newIndex > (size_t)cachedItems->GetLength()) throw IndexOutOfRangeException("Seek out of range");
    
    index = newIndex;
    this->position = index;
    return index;
}