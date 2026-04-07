#pragma once
#include "exceptions.h"
template <class T>
class LinkedList {
   private:

    struct Node {
        T value;
        Node* next;
        Node(const T& val) : value(val), next(nullptr) {}
    };
    Node* head;
    Node* tail;
    int length;

    void Clear();

   public:
    LinkedList(T* items, int count);
    LinkedList();
    LinkedList(const LinkedList<T>& list);
    ~LinkedList();

    void Set(int index, const T& value);
    T GetFirst() const;
    T GetLast() const;
    T Get(int index) const;
    LinkedList<T>* GetSubList(int startIndex, int endIndex) const;
    int GetLength() const;

    void Append(const T& item);
    void Prepend(const T& item);
    void InsertAt(const T& item, int index);
    LinkedList<T>* Concat(LinkedList<T>* list) const;
};

#include "linked_list.tpp"