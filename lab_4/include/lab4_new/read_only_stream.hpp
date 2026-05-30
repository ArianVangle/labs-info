#pragma once

#include "stream.hpp"
#include "lazy_sequence.hpp"
#include <fstream>
#include <sstream>

template<class T>
class SequenceReadStream : public ReadOnlyStream<T> {
private:
    Sequence<T>* source;
    size_t sourceLength;
    bool isOwner;
    
public:
    SequenceReadStream(Sequence<T>* seq, bool takeOwnership = false);
    ~SequenceReadStream() override;
    
    void Open() override;
    void Close() override;
    bool IsEndOfStream() const override;
    T Read() override;
    bool IsCanSeek() const override;
    bool IsCanGoBack() const override;
    size_t Seek(size_t index) override;
};

template<class T>
class LazySequenceReadStream : public ReadOnlyStream<T> {
private:
    LazySequence<T>* source;
    
public:
    LazySequenceReadStream(LazySequence<T>* seq);
    
    void Open() override;
    void Close() override;
    bool IsEndOfStream() const override;
    T Read() override;
    bool IsCanSeek() const override;
    bool IsCanGoBack() const override;
    size_t Seek(size_t index) override;
    size_t GetMaterializedCount() const;
};

class FileStream : public ReadOnlyStream<std::string> {
private:
    std::ifstream file;
    std::string filename;
    std::string (*deserializer)(const std::string&);
    
public:
    FileStream(const std::string& fname, std::string (*deser)(const std::string&));
    ~FileStream();
    
    void Open() override;
    void Close() override;
    bool IsEndOfStream() const override;
    std::string Read() override;
    bool IsCanSeek() const override;
    bool IsCanGoBack() const override;
    size_t Seek(size_t index) override;
};

template<class T>
class StringReadStream : public ReadOnlyStream<T> {
private:
    std::istringstream source;
    T (*deserializer)(const std::string&);

public:
    StringReadStream(const std::string& data, T (*deser)(const std::string&));
    
    void Open() override;
    void Close() override;
    bool IsEndOfStream() const override;
    T Read() override;
    
    bool IsCanSeek() const override { return true; }
    size_t Seek(size_t index) override;
};
#include "read_only_stream.tpp"