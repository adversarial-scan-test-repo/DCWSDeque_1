//
// Created by sabrina on 14.05.18.
//

#ifndef BENCHMARK2_LOCKBASEDQUEUE_H
#define BENCHMARK2_LOCKBASEDQUEUE_H

#include "Node.h"
#include <pthread.h>
#include <mutex>

template <class T>
class LockBasedQueue {

private:
    Node<T>* head;
    Node<T>* tail;

    std::mutex enqueue_mutex_lock;
    std::mutex dequeue_mutex_lock;

    FRIEND_TEST(TestBenchmark2Sequential, InitializedQueueHeadEqualsTail);




public:

    static constexpr T Empty = T();

    LockBasedQueue();

    void enqueue(T elem);
    T dequeue();

};

template<class T>
LockBasedQueue<T>::LockBasedQueue() {
    head = new Node<T>(Empty);
    tail = head;
}

template<class T>
void LockBasedQueue<T>::enqueue(T elem) {
    enqueue_mutex_lock.lock();
    Node<T>* node = new Node<T>(elem);

    tail->setNext(node);
    tail = node;

    enqueue_mutex_lock.unlock();
}

template<class T>
T LockBasedQueue<T>::dequeue() {
    T elem;

    dequeue_mutex_lock.lock();
    if(head == tail) {
        elem = Empty;
    } else {
        elem = head->getNext()->getValue();
        head = head->getNext();
    }
    dequeue_mutex_lock.unlock();
    return elem;
}



#endif //BENCHMARK2_LOCKBASEDQUEUE_H
