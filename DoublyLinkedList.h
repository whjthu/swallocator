#pragma once
#ifndef DOUBLYLINKEDLIST_H
#define DOUBLYLINKEDLIST_H

#include <iostream>

template <class T>
class DoublyLinkedList {
public:
    struct Node {
        Node *prev;
        Node *next;
        T data;
    };

    Node *head;
    Node *tail;

    DoublyLinkedList();

    void insert(Node *previousNode, Node *newNode);
    void remove(Node *deleteNode);

    void push_front(Node *newNode);
    void push_back(Node *newNode);

    Node *pop_front();
    Node *pop_back();

    void print();
};

template <class T>
DoublyLinkedList<T>::DoublyLinkedList() : head(nullptr), tail(nullptr) {}

template <class T>
void DoublyLinkedList<T>::insert(Node* previousNode, Node *newNode) {
    newNode->prev = previousNode;
    if (previousNode == nullptr) {
        if (head != nullptr) {
            head->prev = newNode;
        } else {
            tail = newNode;
        }
        newNode->next = head;
        head = newNode;
    } else {
        if (previousNode->next == nullptr) {
            tail = newNode;
        } else {
            previousNode->next->prev = newNode;
        }
        newNode->prev = previousNode;
        newNode->next = previousNode->next;
        previousNode->next = newNode;
    }
}

template <class T>
void DoublyLinkedList<T>::remove(Node *deleteNode) {
    if (deleteNode->prev == nullptr) {
        head = deleteNode->next;
    } else {
        deleteNode->prev->next = deleteNode->next;
    }
    if (deleteNode->next == nullptr) {
        tail = deleteNode->prev;
    } else {
        deleteNode->next->prev = deleteNode->prev;
    }
} 

template <class T>
void DoublyLinkedList<T>::push_front(Node *newNode) {
    insert(nullptr, newNode);
}

template <class T>
void DoublyLinkedList<T>::push_back(Node *newNode) {
    insert(tail, newNode);
}

template <class T>
typename DoublyLinkedList<T>::Node *DoublyLinkedList<T>::pop_front() {
    auto ret = head;
    if (head != nullptr)
        remove(head);
    return ret;
}

template <class T>
typename DoublyLinkedList<T>::Node *DoublyLinkedList<T>::pop_back() {
    auto ret = tail;
    if (tail != nullptr)
        remove(tail);
    return ret;
}

template <class T>
void DoublyLinkedList<T>::print() {
    auto it = head;
    std::cout << "DLL: ";
    while (it != nullptr) {
        std::cout << it->data;
        if (it != tail)
            std::cout << "->";
        it = it->next;
    }
    std::cout << std::endl;
}
#endif // DOUBLYLINKEDLIST_H
