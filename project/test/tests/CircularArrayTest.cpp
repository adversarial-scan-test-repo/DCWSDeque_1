//
// Created by sabrina on 06.05.18.
//


#include <gtest/gtest.h>
#include "../../src/CircularArray.h"

class CircularArrayTest: public ::testing::Test {
protected:
    virtual void SetUp() {}

    virtual void TearDown(){}
};

TEST_F(CircularArrayTest, ArraySizeWorks) {
    MemoryPool<int> Pool;
    CircularArray<int> array(4, &Pool);

    //Positive test
    EXPECT_EQ(16, array.arraySize());

    //Negative test
    EXPECT_TRUE(4 != array.arraySize());
}

TEST_F(CircularArrayTest, PutAndGetWork) {

    MemoryPool<int> Pool;
    CircularArray<int> array(2, &Pool);
    array.put(0, 1);
    array.put(1, 6);
    array.put(3, 19);
    array.put(2, 12);

    EXPECT_EQ(1, array.get(0));
    EXPECT_EQ(6, array.get(1));
    EXPECT_EQ(12, array.get(2));
    EXPECT_EQ(19, array.get(3));
}

TEST_F(CircularArrayTest, PutOutOfBoundsWrapsAround) {
    MemoryPool<int> Pool;
    CircularArray<int> array(2, &Pool);
    array.put(0, 1);
    array.put(4, 15);

    EXPECT_EQ(15, array.get(0));
}


TEST_F(CircularArrayTest, GrowWorks) {
    MemoryPool<int> Pool;
    CircularArray<int> array(2, &Pool);

    EXPECT_EQ(4, array.arraySize());

    array.put(0, 14);
    array.put(1, 15);
    array.put(2, 16);

    auto biggerArray = array.grow(3, 0);

    EXPECT_EQ(8, biggerArray->arraySize());
    EXPECT_EQ(14, biggerArray->get(0));
    EXPECT_EQ(15, biggerArray->get(1));
    EXPECT_EQ(16, biggerArray->get(2));
}


