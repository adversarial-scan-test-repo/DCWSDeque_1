//
// Created by sabrina on 08.05.18.
//



#include <gtest/gtest.h>
#include "../../src/CircularArray.h"

class CircularArrayShrinkTest: public ::testing::Test {
protected:
    virtual void SetUp() {}

    virtual void TearDown(){}
};

TEST_F(CircularArrayShrinkTest, ShrinkArraySizeOneReturnsSameArray) {
    MemoryPool<int> Pool;
    CircularArray<int> oneElemArray(0, &Pool);

    EXPECT_EQ(1, oneElemArray.arraySize());

    oneElemArray.put(0, 12);

    auto shrunken = oneElemArray.shrink(1, 0);

    EXPECT_EQ(1, shrunken->arraySize());
    EXPECT_EQ(12, shrunken->get(0));


}

TEST_F(CircularArrayShrinkTest, ShrinkArrayWithNoReferenceWorks) {
    MemoryPool<int> Pool;
    CircularArray<int> originalArray(1, &Pool);

    EXPECT_EQ(2, originalArray.arraySize());
    originalArray.put(0, 12);
    originalArray.put(1, 13);


    auto  shrunkenBottom1 = originalArray.shrink(1, 0);

    EXPECT_EQ(1, shrunkenBottom1->arraySize());

    //Note: size is one, we copy to bottom, the element in the array should be the last value to be copied over: 12
    EXPECT_EQ(12, shrunkenBottom1->get(0));


    auto shrunkenBottom2 = originalArray.shrink(2, 0);

    EXPECT_EQ(1, shrunkenBottom2->arraySize());

    //Note: size is one, we copy to bottom, the element in the array should be the last value to be copied over: 13
    EXPECT_EQ(13, shrunkenBottom2->get(0));


}

TEST_F(CircularArrayShrinkTest, ShrinkToOneSmallerArrayWorks) {

    MemoryPool<int> Pool;
    CircularArray<int> smallArray(2, &Pool);
    smallArray.put(0, 14);
    smallArray.put(1, 15);
    smallArray.put(2, 16);


    EXPECT_EQ(4, smallArray.arraySize());

    auto bigArray = smallArray.grow(3, 0);

    EXPECT_EQ(8, bigArray->arraySize());

    auto shrunkMiddleArray = bigArray->shrink(3, 0);

    EXPECT_EQ(4, shrunkMiddleArray->arraySize());
    EXPECT_EQ(14, shrunkMiddleArray->get(0));
}

TEST_F(CircularArrayShrinkTest, ShrinkOverSeveralSmallerArraysWorks) {

    MemoryPool<int> Pool;
    CircularArray<int> smallArray(2, &Pool);
    smallArray.put(0, 14);
    smallArray.put(1, 15);
    smallArray.put(2, 16);

    auto middleArray = smallArray.grow(3, 0);
    auto largeArray = middleArray->grow(3, 0);

    auto shrunken = largeArray->shrink(3, 0);

    EXPECT_EQ(4, shrunken->arraySize());

}

TEST_F(CircularArrayShrinkTest, ShrinkAfterBottomModificationWorks) {

    MemoryPool<int> Pool;
    CircularArray<int> smallArray(2, &Pool);

    smallArray.put(0, 14);
    smallArray.put(1, 15);

    auto middleArray = smallArray.grow(2, 0);
    middleArray->put(2, 16);
    auto shrunken = middleArray->shrink(3, 0);

    EXPECT_EQ(16, shrunken->get(2));

}


TEST_F(CircularArrayShrinkTest, ShrinkAfterElemModificationWorks) {

    MemoryPool<int> Pool;
    CircularArray<int> smallArray(2, &Pool);

    smallArray.put(0, 14);
    smallArray.put(1, 15);

    auto middleArray = smallArray.grow(2, 0);
    middleArray->put(0, 16);
    auto shrunken = middleArray->shrink(1, 0);

    EXPECT_EQ(16, shrunken->get(0));
    EXPECT_EQ(15, shrunken->get(1));
}

TEST_F(CircularArrayShrinkTest, ShrinkOverSeveralSmallerArraysWithSmallestTooSmallWorks) {

    MemoryPool<int> Pool;
    CircularArray<int> smallArray(2, &Pool);
    smallArray.put(0, 14);
    smallArray.put(1, 15);
    smallArray.put(2, 16);
    smallArray.put(3, 17);

    auto middleArray = smallArray.grow(3, 0);
    middleArray->put(4, 18);
    middleArray->put(5, 19);
    middleArray->put(6, 20);
    middleArray->put(7, 21);

    auto largeArray = middleArray->grow(3, 0);

    auto shrunken = largeArray->shrink(7, 0);

    EXPECT_EQ(8, shrunken->arraySize());

}