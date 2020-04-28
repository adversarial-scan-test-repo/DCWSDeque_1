//
// Created by sabrina on 14.05.18.
//

#include <gtest/gtest.h>
#include <thread>
#include "../../src/LockBasedQueue.h"

class TestBenchmark2Threads: public ::testing::Test {
protected:
    virtual void SetUp(){}
    virtual void TearDown(){}

public:
    static void enqueueElem(LockBasedQueue<int> &queue, int elem);
    static void dequeueElem(LockBasedQueue<int> &queue, int &elem);
};

void TestBenchmark2Threads::enqueueElem(LockBasedQueue<int> &queue, int elem) {
    queue.enqueue(elem);
}

void TestBenchmark2Threads::dequeueElem(LockBasedQueue<int> &queue, int &elem) {
    elem = queue.dequeue();
}


TEST_F(TestBenchmark2Threads, TwoThreadsEnqWorks) {

    LockBasedQueue<int> queue;

    //note: always wrap passages by reference in std::ref, otherwise threads make copies
    std::thread t1 = std::thread(enqueueElem, std::ref(queue), 1);
    std::thread t2 = std::thread(enqueueElem, std::ref(queue), 2);

    t1.join();
    t2.join();

    int elem1 = queue.dequeue();
    int elem2 = queue.dequeue();

    EXPECT_TRUE(elem1 == 1 || elem1 == 2);
    EXPECT_TRUE((elem2 == 1 || elem2 == 2) && elem2 != elem1);
}


TEST_F(TestBenchmark2Threads, TwoThreadsDeqWorks) {

    LockBasedQueue<int> queue;

    queue.enqueue(1);
    queue.enqueue(2);

    int elem1;
    int elem2;

    std::thread t1 = std::thread(dequeueElem, std::ref(queue), std::ref(elem1));
    std::thread t2 = std::thread(dequeueElem, std::ref(queue), std::ref(elem2));

    t1.join();
    t2.join();


    EXPECT_TRUE(elem1 == 1 || elem1 == 2);
    EXPECT_TRUE((elem2 == 1 || elem2 == 2) && elem2 != elem1);
}

TEST_F(TestBenchmark2Threads, EnqDeqWorks) {

    LockBasedQueue<int> queue;

    queue.enqueue(1);

    int elem1;

    std::thread t1 = std::thread(enqueueElem, std::ref(queue), 2);
    std::thread t2 = std::thread(dequeueElem, std::ref(queue), std::ref(elem1));

    t1.join();
    t2.join();

    int elem2 = queue.dequeue();

    EXPECT_TRUE((elem1 == 2 && elem2 == 1) || (elem1 == 1 && elem2 == 2));

}
