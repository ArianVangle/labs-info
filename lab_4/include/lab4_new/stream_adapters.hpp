#pragma once

#include "stream.hpp"
#include "read_only_stream.hpp"
#include "../lab3_base/array_sequence.h"
#include <string>

template<class T>
class CsvReadStream : public ReadOnlyStream<T> {
private:
    ReadOnlyStream<std::string>* fileStream;
    T (*parser)(Sequence<std::string>*);
    char delimiter;
    
public:
    CsvReadStream(ReadOnlyStream<std::string>* stream, T (*parseFunc)(Sequence<std::string>*), char delim = ',');
    ~CsvReadStream();
    
    void Open() override;
    void Close() override;
    bool IsEndOfStream() const override;
    T Read() override;
    
    bool IsCanSeek() const override { return fileStream->IsCanSeek(); }
    bool IsCanGoBack() const override { return fileStream->IsCanGoBack(); }
    size_t Seek(size_t index) override;
};

template<class T>
class JsonReadStream : public ReadOnlyStream<T> {
private:
    std::ifstream file;
    std::string filename;
    Sequence<T>* cachedItems;
    size_t index;
public:
    JsonReadStream(const std::string& fname);
    ~JsonReadStream();
    
    void Open() override;
    void Close() override;
    bool IsEndOfStream() const override;
    T Read() override;
    bool IsCanSeek() const override { return true; }
    bool IsCanGoBack() const override { return true; }
    size_t Seek(size_t newIndex) override;
};

#include "stream_adapters.tpp"