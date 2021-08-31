#ifndef STACKLINKEDLIST_H
#define STACKLINKEDLIST_H

template <class T>
class StackLinkedList {
public:
    struct Node {
        T data;
        Node *next;
    };

    Node *head;

    StackLinkedList() = default;
    StackLinkedList(StackLinkedList &stackLinkedList) = delete;
    void push(Node *newNode);
    Node *pop();
};

template <class T>
void StackLinkedList::push(Node *newNode) {
    newNode->next = head;
    head = newNode;
}

template <class T>
typename StackLinkedList<T>::Node *StackLinkedList::pop() {
    Node *top = head;
    head = head->next;
    return top;
}


#endif // STACKLINKEDLIST_H
