#include "DoublyLinkedList.h"

using Node = DoublyLinkedList<int>::Node;

int main() {
    auto dll = DoublyLinkedList<int>();
    auto n1 = new Node();
    n1->data = 1;
    dll.insert(nullptr, n1);
    dll.print();
    auto n2 = new Node();
    n2->data = 2;
    dll.insert(nullptr, n2);
    dll.print();
    auto n3 = new Node();
    n3->data = 3;
    dll.insert(n2, n3);
    dll.print();
    auto n4 = new Node();
    n4->data = 4;
    dll.insert(n1, n4);
    dll.print();
    dll.remove(n2);
    dll.print();
    dll.insert(n4, n2);
    dll.print();
    dll.remove(n3);
    dll.print();
    dll.insert(n4, n3);
    dll.print();
    dll.remove(n1);
    dll.print();
    while (dll.head != nullptr) {
        dll.pop_front();
        dll.print();
    }
    delete n1;
    delete n2;
    delete n3;
    delete n4;
    return 0;
}
