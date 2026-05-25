#pragma once

#include <string>

class Ring {
   private:
    int size;
    std::string color;
    static int nextId;
    int id;

   public:
    Ring(int s = 1, const std::string& c = "\033[96m") : size(s), color(c), id(++nextId) {}
    Ring(const Ring& other) : size(other.size), color(other.color), id(++nextId) {}
    ~Ring() {}

    int GetSize() const {
        return size;
    }

    std::string GetColor() const {
        return color;
    }

    int GetId() const {
        return id;
    }

    void SetSize(int s) {
        size = s;
    }

    void SetColor(const std::string& c) {
        color = c;
    }

    bool operator<(const Ring& other) const {
        return size < other.size;
    }

    bool operator>(const Ring& other) const {
        return size > other.size;
    }

    bool operator==(const Ring& other) const {
        return size == other.size;
    }

    bool operator!=(const Ring& other) const {
        return size != other.size;
    }
    

    Ring& operator=(const Ring& other) {
        if (this != &other) {
            size = other.size;
            color = other.color;
        }
        return *this;
    }

    static void ResetIdCounter() {
        nextId = 0;
    }
};

inline int Ring::nextId = 0;