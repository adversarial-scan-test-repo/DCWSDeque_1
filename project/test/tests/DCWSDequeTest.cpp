//
// Created by sabrina on 06.05.18.
//

#include <gtest/gtest.h>
#include "../../src/DCWSDeque.h"
#include "../../src/MemoryPool.h"
#include <cmath>

class DCWSDequeTest: public ::testing::Test {
protected:
    void SetUp() override {}

    void TearDown() override {}
};


TEST_F(DCWSDequeTest, PushPoPBottomSingleThread){
    auto  Pool = new MemoryPool<double>();
    DCWSDeque<double> dcwsq(Pool);

    for(int k = 0; k < 8; ++k){
        const double elem = sin(k*M_PI_4);
        dcwsq.pushBottom(elem);
    }

    auto elem = dcwsq.popBottom();

    for(int k = 7; elem != DCWSDeque<double>::Empty; --k ){
        EXPECT_DOUBLE_EQ(elem, sin(k*M_PI_4));
        elem = dcwsq.popBottom();

    }
}

TEST_F(DCWSDequeTest, PoppingEmptyReturnsEmpty) {
    auto  Pool = new MemoryPool<double>();
    DCWSDeque<double> deque(Pool);

    EXPECT_TRUE(deque.Empty == deque.popBottom());

    deque.pushBottom(4);

    EXPECT_TRUE(deque.Empty != deque.popBottom());
}

TEST_F(DCWSDequeTest, DequeGrowsItsArrayAndShrinksItBack) {

    auto  Pool = new MemoryPool<double>();
    DCWSDeque<double> dcwsq(Pool);

    EXPECT_EQ(NB_ELEMENTS_IN_POOL_ELEM, dcwsq.activeArray->arraySize());

    for(int k = 0; k < 8; ++k){
        const double elem = sin(k*M_PI_4);
        dcwsq.pushBottom(elem);
    }

    //Grown twice to accomodate 8 objects (Reminder: one cell always kept empty, so we need 9)
            //We assume here the initial log-size is 2
    EXPECT_EQ(4*NB_ELEMENTS_IN_POOL_ELEM, dcwsq.activeArray->arraySize());

    auto elem = dcwsq.popBottom();

    for(int k = 7; elem != DCWSDeque<double>::Empty; --k ){
        EXPECT_DOUBLE_EQ(elem, sin(k*M_PI_4));
        elem = dcwsq.popBottom();

    }

    EXPECT_EQ(NB_ELEMENTS_IN_POOL_ELEM, dcwsq.activeArray->arraySize());

}

TEST_F(DCWSDequeTest, MandatoryCastopTest) {

    auto  Pool = new MemoryPool<double>();
    DCWSDeque<double> deque(Pool);

    long oldTrue = 0l;
    long newVal = 7l;

   EXPECT_TRUE(deque.casTop(oldTrue, newVal));
   EXPECT_TRUE(!deque.casTop(oldTrue, newVal));

}

TEST_F(DCWSDequeTest, StealReturnsEmptyOnEmptyVictim) {

    auto  Pool = new MemoryPool<double>();
    DCWSDeque<double> deque(Pool);

    int stolen = deque.steal();
    EXPECT_TRUE(stolen == deque.Empty);
}

TEST_F(DCWSDequeTest, StealReturnsOnlyElemOfVictimAndVictimEmptied) {

    auto  Pool = new MemoryPool<double>();
    DCWSDeque<double> deque(Pool);
    deque.pushBottom(9);

    int stolen = deque.steal();
    EXPECT_EQ(9, stolen);
    EXPECT_TRUE(deque.popBottom() == deque.Empty);
}

TEST_F(DCWSDequeTest, StealReturnsOldestElem) {

    auto  Pool = new MemoryPool<double>();
    DCWSDeque<double> deque(Pool);
    deque.pushBottom(9);
    deque.pushBottom(10);
    deque.pushBottom(11);

    int stolen = deque.steal();
    EXPECT_EQ(9, stolen);
    EXPECT_EQ(11, deque.popBottom());
}

TEST_F(DCWSDequeTest, DequeGrowsStealsShrinks){
    auto  Pool = new MemoryPool<double>();
    DCWSDeque<double> deque(Pool);

    deque.pushBottom(2);
    deque.pushBottom(3);
    deque.pushBottom(5);

    deque.pushBottom(7);
    deque.pushBottom(11);
    deque.pushBottom(13);
    deque.pushBottom(17);
    deque.pushBottom(19);

    auto stolen = deque.steal();
    EXPECT_EQ(2, stolen);
    stolen = deque.steal();
    EXPECT_EQ(3, stolen);
    stolen = deque.steal();
    EXPECT_EQ(5, stolen);
    stolen = deque.steal();
    EXPECT_EQ(7, stolen);
    stolen = deque.steal();
    EXPECT_EQ(11, stolen);
    auto poped = deque.popBottom();

    EXPECT_EQ(4, deque.activeArray->arraySize());
    EXPECT_EQ(19, poped);

}

