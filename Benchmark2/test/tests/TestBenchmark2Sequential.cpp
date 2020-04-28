//
// Created by sabrina on 11.05.18.
//

#include <gtest/gtest.h>
#include "../../src/LockBasedQueue.h"

class TestBenchmark2Sequential: public ::testing::Test {
protected:
    virtual void SetUp() {}

    virtual void TearDown(){}
};

TEST_F(TestBenchmark2Sequential, InitializedEmptyQueueHasOnlyEmptyElem) {

    LockBasedQueue<int> queue;

    int elem = queue.dequeue();

    EXPECT_TRUE(elem == queue.Empty);
}

TEST_F(TestBenchmark2Sequential, InitializedQueueHeadEqualsTail) {

    LockBasedQueue<int> queue;

    EXPECT_TRUE(queue.head = queue.tail);
}

TEST_F(TestBenchmark2Sequential, EnqueueWorksOneElem) {

    LockBasedQueue<int> queue;

    queue.enqueue(4);

    EXPECT_EQ(4, queue.dequeue());
    EXPECT_TRUE(queue.dequeue() == queue.Empty);

}

TEST_F(TestBenchmark2Sequential, EnqueueWorksMultipleElems) {

    LockBasedQueue<int> queue;

    queue.enqueue(4);
    queue.enqueue(5);
    queue.enqueue(6);

    EXPECT_EQ(4, queue.dequeue());
    EXPECT_EQ(5, queue.dequeue());
    EXPECT_EQ(6, queue.dequeue());
    EXPECT_TRUE(queue.dequeue() == queue.Empty);
    EXPECT_TRUE(queue.dequeue() == queue.Empty);

}

