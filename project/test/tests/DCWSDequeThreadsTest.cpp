//
// Created by sabrina on 20.05.18.
//

#include <gtest/gtest.h>
#include <thread>
#include "../../src/MemoryPool.h"
#include "../../src/DCWSDeque.h"

class DCWSDequeThreadsTest: public ::testing::Test {
protected:
    void SetUp() override {}

    void TearDown() override {}

public:
    static void pushElem(DCWSDeque<int> &deque, int elem);
    static void popElem(DCWSDeque<int> &deque, int &elem);
    static void stealElem(DCWSDeque<int> &deque, int &elem);
};

void DCWSDequeThreadsTest::pushElem(DCWSDeque<int> &deque, int elem) {
    deque.pushBottom(elem);
}

void DCWSDequeThreadsTest::popElem(DCWSDeque<int> &deque, int &elem) {
    elem = deque.popBottom();
}

void DCWSDequeThreadsTest::stealElem(DCWSDeque<int> &deque, int &elem) {
    elem = deque.steal();
}


TEST_F(DCWSDequeThreadsTest, PushbottomTwiceWorks) {

    auto  Pool = new MemoryPool<int>();
    DCWSDeque<int> deque(Pool);

    //note: always wrap passages by reference in std::ref, otherwise threads make copies
    std::thread t1 = std::thread(pushElem, std::ref(deque), 1);
    std::thread t2 = std::thread(pushElem, std::ref(deque), 2);

    t1.join();
    t2.join();

    int elem1 = deque.popBottom();
    int elem2 = deque.popBottom();

    EXPECT_TRUE((elem1 == 1 && elem2 == 2) ||(elem1 == 2 && elem2 == 1 ));
}

TEST_F(DCWSDequeThreadsTest, PopBottomTwiceWorks) {

    auto  Pool = new MemoryPool<int>();
    DCWSDeque<int> deque(Pool);

    deque.pushBottom(1);
    deque.pushBottom(2);

    int elem1;
    int elem2;

    //note: always wrap passages by reference in std::ref, otherwise threads make copies
    std::thread t1 = std::thread(popElem, std::ref(deque), std::ref(elem1));
    std::thread t2 = std::thread(popElem, std::ref(deque), std::ref(elem2));

    t1.join();
    t2.join();


    EXPECT_TRUE((elem1 == 1 && elem2 == 2) ||(elem1 == 2 && elem2 == 1 ));
}

TEST_F(DCWSDequeThreadsTest, PushAndPopWorks) {

    auto  Pool = new MemoryPool<int>();
    DCWSDeque<int> deque(Pool);

    deque.pushBottom(1);

    int elem1;

    //note: always wrap passages by reference in std::ref, otherwise threads make copies
    std::thread t1 = std::thread(popElem, std::ref(deque), std::ref(elem1));
    std::thread t2 = std::thread(pushElem, std::ref(deque), 2);

    t1.join();
    t2.join();

    int elem2 = deque.popBottom();

    /*
     * t1 first:
     * deque = {1} -> deque = {}, elem1 = 1, -> deque = {2}, elem1 = 1 -> deque = {}, elem1 = 1, elem2 = 2
     *
     * t2 first:
     * deque = {1} -> deque = {2,1} -> deque = {1}, elem1 = 2 -> deque = {}, elem1 = 2, elem2 = 1
     *
     */


    EXPECT_TRUE((elem1 == 1 && elem2 == 2) ||(elem1 == 2 && elem2 == 1 ));
}


TEST_F(DCWSDequeThreadsTest, twoStealsWorks) {
    auto  Pool = new MemoryPool<int>();
    DCWSDeque<int> deque(Pool);

    deque.pushBottom(1);
    deque.pushBottom(2);

    int elem1;
    int elem2;

    //note: always wrap passages by reference in std::ref, otherwise threads make copies
    std::thread t1 = std::thread(stealElem, std::ref(deque), std::ref(elem1));
    std::thread t2 = std::thread(stealElem, std::ref(deque), std::ref(elem2));

    t1.join();
    t2.join();


    EXPECT_TRUE((elem1 == 1 && elem2 == 2) ||(elem1 == 2 && elem2 == 1 ));
}

TEST_F(DCWSDequeThreadsTest, stealAndPushWorks) {
    auto  Pool = new MemoryPool<int>();
    DCWSDeque<int> deque(Pool);

    deque.pushBottom(1);

    int elem1;

    //note: always wrap passages by reference in std::ref, otherwise threads make copies
    std::thread t1 = std::thread(stealElem, std::ref(deque), std::ref(elem1));
    std::thread t2 = std::thread(pushElem, std::ref(deque), 2);

    t1.join();
    t2.join();

    int elem2 = deque.popBottom();

    /*
     * t1 first:
     * deque = {1} -> deque = {}, elem1 = 1, -> deque = {2}, elem1 = 1 -> deque = {}, elem1 = 1, elem2 = 2
     *
     * t2 first:
     * deque = {1} -> deque = {2,1} -> deque = {2}, elem1 = 1 -> deque = {}, elem1 = 1, elem2 = 2
     *
     */


    EXPECT_TRUE((elem1 == 1 && elem2 == 2));
}

TEST_F(DCWSDequeThreadsTest, stealAndPopWorks) {

    auto  Pool = new MemoryPool<int>();
    DCWSDeque<int> deque(Pool);

    deque.pushBottom(1);
    deque.pushBottom(2);

    int elem1;
    int elem2;

    //note: always wrap passages by reference in std::ref, otherwise threads make copies
    std::thread t1 = std::thread(stealElem, std::ref(deque), std::ref(elem1));
    std::thread t2 = std::thread(popElem, std::ref(deque), std::ref(elem2));

    t1.join();
    t2.join();



    EXPECT_TRUE((elem1 == 2 && elem2 == 1) || (elem1 == 1 && elem2 == 2));
}

TEST_F(DCWSDequeThreadsTest, stealAndPushAndPopWorks) {
    auto  Pool = new MemoryPool<int>();
    DCWSDeque<int> deque(Pool);

    deque.pushBottom(1);
    deque.pushBottom(2);

    int elem1;
    int elem2;

    //note: always wrap passages by reference in std::ref, otherwise threads make copies
    std::thread t1 = std::thread(stealElem, std::ref(deque), std::ref(elem1));
    std::thread t2 = std::thread(popElem, std::ref(deque), std::ref(elem2));
    std::thread t3 = std::thread(pushElem, std::ref(deque), 3);

    t1.join();
    t2.join();
    t3.join();

    int elem3 = deque.popBottom();

    EXPECT_TRUE((elem1 == 1 && elem2 == 2 && elem3 == 3) ||
                (elem1 == 1 && elem2 == 3 && elem3 == 2 )||
                (elem1 == 2 && elem2 == 1 && elem3 == 3 )||
                (elem1 == 2 && elem2 == 3 && elem3 == 1 )||
                (elem1 == 3 && elem2 == 1 && elem3 == 2 )||
                (elem1 == 3 && elem2 == 2 && elem3 == 1 ));
}


