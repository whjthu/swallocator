#ifndef SINGLYLINKEDLIST_H
#define SINGLYLINKEDLIST_H

template <class T>
class SinglyLinkedList {
public:
    struct Node {
        T data;
        Node *next;
    };

    Node *head;

    SinglyLinkedList();

    void insert(Node *previousNode, Node *newNode);
    void remove(Node *previousNode, Node *deleteNode);
};

template <class T>
SinglyLinkedList<T>::SinglyLinkedList() {}

template <class T>
void SinglyLinkedList<T>::insert(Node* previousNode, Node *newNode) {
    if (previousNode == nullptr) {
        if (head != nullptr) {
            newNode->next = head;
        } else {
            newNode->next = nullptr;
        }
        head = newNode;
    } else {
        if (previousNode->next == nullptr) {
            previousNode->next = newNode;
            newNode->next = nullptr;
        } else {
            newNode->next = previousNode->next;
            previousNode->next = newNode;
        }
    }
}

template <class T>
void SinglyLinkedList<T>::remove(Node* previousNode, Node *deleteNode) {
    if (previousNode == nullptr) {
        head = deleteNode->next;
    } else {
        previousNode->next = deleteNode->next;
    }
}

#endif // SINGLYLINKEDLIST_H
