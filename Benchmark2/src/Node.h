//
// Created by sabrina on 14.05.18.
//

#ifndef BENCHMARK2_NODE_H
#define BENCHMARK2_NODE_H

#include <clocale>

template <class T>
class Node {
private:
    T value;
    Node* next;


public:

    Node(T value) : value(value) {
        next = NULL;
    }

    T getValue() const {
        return value;
    }

    void setValue(T value) {
        Node::value = value;
    }

    Node *getNext() const {
        return next;
    }

    void setNext(Node *next) {
        Node::next = next;
    }


};


#endif //BENCHMARK2_NODE_H
