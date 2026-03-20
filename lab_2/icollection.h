#pragma once
#include <cstddef>

template <class T>
class ICollection {
   public:
    virtual ~ICollection() {}

    virtual T Get(size_t index) const = 0;
    virtual size_t GetCount() const = 0;
    virtual ICollection<T>* Clone() const = 0;
};