#pragma once

#include "write_only_stream.hpp"
#include "stream.hpp"
#include "../lab3_base/sequence.h"
#include <fstream>
#include <string>

template<class T>
class SequenceWriteStream : public WriteOnlyStream<T> {
private:
    Sequence<T>* target;
public:
    SequenceWriteStream(Sequence<T>* seq);
    void Open() override;
    void Close() override;
    bool IsEndOfStream() const override;
    size_t Write(T item) override;
    bool IsCanSeek() const override;
    bool IsCanGoBack() const override;
};

template<class T>
class FileWriteStream : public WriteOnlyStream<T> {
private:
    std::ofstream file;
    std::string filename;
    std::string (*serializer)(T);
public:
    FileWriteStream(const std::string& fname, std::string (*ser)(T));
    ~FileWriteStream();
    void Open() override;
    void Close() override;
    size_t Write(T item) override;
    bool IsCanSeek() const override;
    bool IsCanGoBack() const override;
};

#include "write_only_stream.tpp"