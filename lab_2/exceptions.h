#pragma once
#include <exception>
#include <string>

class IndexOutOfRangeException : public std::exception {
   private:
    std::string message;

   public:
    IndexOutOfRangeException(const std::string& msg = "Index out of range")
        : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class InvalidOperationException : public std::exception {
   private:
    std::string message;

   public:
    InvalidOperationException(const std::string& msg = "Invalid operation")
        : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};