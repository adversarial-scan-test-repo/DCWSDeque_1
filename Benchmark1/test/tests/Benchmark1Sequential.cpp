//
// Created by lu on 25.05.18.
//

#include <gtest/gtest.h>
#include <math.h>
#include "BoundedDeque.hpp"

class Benchmark1Sequential: public ::testing::Test{
protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(Benchmark1Sequential, PushPopElement){
    BoundedDeque<double> deque(5);

    double elem = M_PI;

    deque.push_bottom(elem);

    EXPECT_DOUBLE_EQ(M_PI, deque.pop_bottom());

}

TEST_F(Benchmark1Sequential, EmptyPop){
    BoundedDeque<double> deuque(4);

    double elem = deuque.pop_bottom();

    EXPECT_DOUBLE_EQ(elem, deuque.Empty);
}

TEST_F(Benchmark1Sequential, EmptySteal){
    BoundedDeque<double> deque(6);

    auto elem = deque.pop_top();

    EXPECT_DOUBLE_EQ(elem, BoundedDeque<double>::Empty);
}

TEST_F(Benchmark1Sequential, FullDeque){
    BoundedDeque<double> deque(4);

    ASSERT_TRUE(deque.push_bottom(0));
    ASSERT_TRUE(deque.push_bottom(1));
    ASSERT_TRUE(deque.push_bottom(2));
    ASSERT_TRUE(deque.push_bottom(3));
    ASSERT_FALSE(deque.push_bottom(4));
}

TEST_F(Benchmark1Sequential, PushPopMultiple){
    BoundedDeque<int> deque(13);

    //Fib Sequence
    std::vector<int> fib_seq;
    int i = 1, old_i = 1;
    while(i <= 233 ){
        fib_seq.push_back(i);
        deque.push_bottom(i);
        auto i_tmp = i;
        i = i_tmp + old_i;
        old_i = i_tmp;
    }

    std::for_each(fib_seq.rbegin(), fib_seq.rend(), [&deque](int& elem)
        {
            EXPECT_EQ(elem, deque.pop_bottom());

        });
}

TEST_F(Benchmark1Sequential, PushStealMultiple){
    BoundedDeque<int> deque(13);

    std::vector<int> fib_seq;
    int i = 1, old_i = 1;
    while(i <= 233){
        fib_seq.push_back(i);
        deque.push_bottom(i);
        auto i_tmp = i;
        i = i_tmp + old_i;
        old_i = i_tmp;
    }

    std::for_each(fib_seq.begin(), fib_seq.end(), [&deque](int& elem)
        {
            EXPECT_EQ(elem, deque.pop_top());

        });
}


TEST_F(Benchmark1Sequential, MixedMultiple){
    BoundedDeque<int> deque(10);

    //primes
    const std::vector<int> primes = {2,3,5,7,11,13,17,19,23,29};
    ASSERT_TRUE(deque.push_bottom(primes[0]));
    ASSERT_TRUE(deque.push_bottom(primes[1]));

    EXPECT_EQ(primes[0], deque.pop_top());

    ASSERT_TRUE(deque.push_bottom(primes[2]));

    EXPECT_EQ(primes[2], deque.pop_bottom());

    ASSERT_TRUE(deque.push_bottom(primes[3]));
    ASSERT_TRUE(deque.push_bottom(primes[4]));
    ASSERT_TRUE(deque.push_bottom(primes[5]));

    EXPECT_EQ(primes[1], deque.pop_top());

    ASSERT_TRUE(deque.push_bottom(primes[6]));

    EXPECT_EQ(primes[3], deque.pop_top());

    ASSERT_TRUE(deque.push_bottom(primes[7]));
    ASSERT_TRUE(deque.push_bottom(primes[8]));
    ASSERT_TRUE(deque.push_bottom(primes[9]));


    EXPECT_EQ(primes[9], deque.pop_bottom());
    EXPECT_EQ(primes[8], deque.pop_bottom());




}

