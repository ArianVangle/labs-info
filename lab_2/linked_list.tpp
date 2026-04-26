#pragma once
#include "linked_list.h"

template <class T>
void LinkedList<T>::Clear() {
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

template <class T>
LinkedList<T>::LinkedList(T* items, int count) : head(nullptr), tail(nullptr), length(0) {
    for (int i = 0; i < count; i++) 
        Append(items[i]);
}

template <class T>
LinkedList<T>::LinkedList() : head(nullptr), tail(nullptr), length(0) {
}

template <class T>
LinkedList<T>::LinkedList(const LinkedList<T>& list) : head(nullptr), tail(nullptr), length(0) {
    Node* current = list.head;
    while (current != nullptr) {
        Append(current->value);
        current = current->next;
    }
}

template <class T>
LinkedList<T>::~LinkedList() {
    Clear();
}

template <class T>
void LinkedList<T>::Set(int index, const T& value) {
    if (index < 0 || index >= length) throw IndexOutOfRangeException("Index out of range");
    Node* current = head;
    for (int i = 0; i < index; i++) 
        current = current->next;
    current->value = value;
}

template <class T>
T LinkedList<T>::GetFirst() const {
    if (length == 0) 
        throw IndexOutOfRangeException("List is empty");
    return head->value;
}

template <class T>
T LinkedList<T>::GetLast() const {
    if (length == 0) 
        throw IndexOutOfRangeException("List is empty");
    return tail->value;
}

template <class T>
T LinkedList<T>::Get(int index) const {
    if (index < 0 || index >= length) 
        throw IndexOutOfRangeException("Index out of range");
    Node* current = head;

    for (int i = 0; i < index; i++) 
        current = current->next;
    return current->value;
}

template <class T>
LinkedList<T>* LinkedList<T>::GetSubList(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= length || startIndex > endIndex)
        throw IndexOutOfRangeException("Invalid indices for sublist");
    LinkedList<T>* result = new LinkedList<T>();
    Node* current = head;
    for (int i = 0; i < startIndex; i++) 
        current = current->next;
    for (int i = startIndex; i <= endIndex; i++) {
        result->Append(current->value);
        current = current->next;
    }
    return result;
}

template <class T>
int LinkedList<T>::GetLength() const {
    return length;
}

template <class T>
void LinkedList<T>::Append(const T& item) {
    Node* newNode = new Node(item);
    if (length == 0) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    length++;
}

template <class T>
void LinkedList<T>::Prepend(const T& item) {
    Node* newNode = new Node(item);
    if (length == 0) {
        head = tail = newNode;
    } else {
        newNode->next = head;
        head = newNode;
    }
    length++;
}

template <class T>
void LinkedList<T>::InsertAt(const T& item, int index) {
    if (index < 0 || index >= length) 
        throw IndexOutOfRangeException("Index out of range");
    if (index == 0) {
        Prepend(item);
        return;
    }
    Node* current = head;
    for (int i = 0; i < index - 1; i++) 
        current = current->next;
    Node* newNode = new Node(item);
    newNode->next = current->next;
    current->next = newNode;
    length++;
}

template <class T>
LinkedList<T>* LinkedList<T>::Concat(const LinkedList<T>& list) const {
    LinkedList<T>* result = new LinkedList<T>(*this);
    Node* current = list.head;
    while (current != nullptr) {
        result->Append(current->value);
        current = current->next;
    }
    return result;
}