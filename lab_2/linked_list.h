#pragma once

#include "exceptions.h"

template <class T>
class LinkedList {
   private:
    struct Node {
        T value;
        Node* next;
        Node(T val) : value(val), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    int length;

    void Clear() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
        tail = nullptr;
        length = 0;
    }

   public:
    LinkedList(T* items, int count) {
        head = nullptr;
        tail = nullptr;
        length = 0;
        for (int i = 0; i < count; i++) {
            Append(items[i]);
        }
    }

    LinkedList() {
        head = nullptr;
        tail = nullptr;
        length = 0;
    }

    LinkedList(const LinkedList<T>& list) {
        head = nullptr;
        tail = nullptr;
        length = 0;
        Node* current = list.head;
        while (current != nullptr) {
            Append(current->value);
            current = current->next;
        }
    }

    ~LinkedList() { Clear(); }

    T GetFirst() const {
        if (length == 0) throw IndexOutOfRangeException("List is empty");
        return head->value;
    }

    T GetLast() const {
        if (length == 0) throw IndexOutOfRangeException("List is empty");
        return tail->value;
    }

    T Get(int index) const {
        if (index < 0 || index >= length)
            throw IndexOutOfRangeException("Index out of range");
        Node* current = head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current->value;
    }

    LinkedList<T>* GetSubList(int startIndex, int endIndex) const {
        if (startIndex < 0 || endIndex >= length || startIndex > endIndex) {
            throw IndexOutOfRangeException("Invalid indices for sublist");
        }
        LinkedList<T>* result = new LinkedList<T>();
        Node* current = head;
        for (int i = 0; i < startIndex; i++) current = current->next;
        for (int i = startIndex; i <= endIndex; i++) {
            result->Append(current->value);
            current = current->next;
        }
        return result;
    }

    int GetLength() const { return length; }

    void Append(T item) {
        Node* newNode = new Node(item);
        if (length == 0) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        length++;
    }

    void Prepend(T item) {
        Node* newNode = new Node(item);
        if (length == 0) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head = newNode;
        }
        length++;
    }

    void InsertAt(T item, int index) {
        if (index < 0 || index >= length)
            throw IndexOutOfRangeException("Index out of range");
        if (index == 0) {
            Prepend(item);
            return;
        }
        Node* current = head;
        for (int i = 0; i < index - 1; i++) {
            current = current->next;
        }
        Node* newNode = new Node(item);
        newNode->next = current->next;
        current->next = newNode;
        length++;
    }

    LinkedList<T>* Concat(LinkedList<T>* list) const {
        LinkedList<T>* result = new LinkedList<T>(*this);
        Node* current = list->head;
        while (current != nullptr) {
            result->Append(current->value);
            current = current->next;
        }
        return result;
    }
};