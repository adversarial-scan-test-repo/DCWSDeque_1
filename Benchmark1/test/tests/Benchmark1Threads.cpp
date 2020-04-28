//
// Created by lu on 25.05.18.
//

#include <gtest/gtest.h>
#include <thread>
#include "BoundedDeque.hpp"

class Benchmark1Threads : public ::testing::Test{
protected:
    virtual void SetUp() {}
    virtual void TearDown() {}

public:
    static void enqueElem(BoundedDeque<int> &deque, int elem);
    static void stealElem(BoundedDeque<int> &deque, int& elem);
};

void Benchmark1Threads::enqueElem(BoundedDeque<int> &deque, int elem) {
    deque.push_bottom(elem);
}

void Benchmark1Threads::stealElem(BoundedDeque<int> &deque, int& elem) {
    elem = deque.pop_top();
}


TEST_F(Benchmark1Threads, TwoThreadsEnqWorks){

    BoundedDeque<int> deque1(5);
    BoundedDeque<int> deque2(5);


    std::thread t1 = std::thread(enqueElem, std::ref(deque1), 2);
    std::thread t2 = std::thread(enqueElem, std::ref(deque2), 3);

    t1.join();
    t2.join();

    EXPECT_EQ(2, deque1.pop_bottom());
    EXPECT_EQ(3, deque2.pop_bottom());

}

TEST_F(Benchmark1Threads, TwoThreadsDeqWorks){
    size_t deque_size = 100;
    BoundedDeque<int> deque1(deque_size);
    

    auto push = [deque_size](BoundedDeque<int>& deque){
        for(int i = 0; i < deque_size; ++i){
            deque.push_bottom(i);
        }
    };

    auto pop = [deque_size](BoundedDeque<int>& deque){
        for(int i = deque_size - 1; i >= 0; --i){
            auto elem = deque.pop_bottom();
            EXPECT_TRUE(i ==  elem || elem == deque.Empty);
        }
    };

    auto pushAndPop = [push, pop](BoundedDeque<int>& deque, bool& guard){
        while(!guard){}; //sync label
        push(deque);
        pop(deque);
    };

    auto steal = [deque_size](BoundedDeque<int>& deque, bool& guard){
        while(!guard){}; //sync label

        for(int i = 0; i < deque_size; ++i){
            auto elem = deque.pop_top();
            EXPECT_TRUE(i == elem || elem == deque.Empty);
        }
    };

    bool gate = false;
    std::thread t1 = std::thread(pushAndPop, std::ref(deque1), std::ref(gate));
    std::thread t2 = std::thread(steal, std::ref(deque1), std::ref(gate));
    gate = true;

    t1.join();
    t2.join();

}

TEST_F(Benchmark1Threads, TwoThreadsStealEachOther){
    size_t deque_size = 100;
    const double eps = 0.001;
    BoundedDeque<double> deque1(deque_size);
    BoundedDeque<double> deque2(deque_size);

    auto push = [deque_size](BoundedDeque<double>& deque){
        for(int i = 0; i < deque_size; ++i){
            deque.push_bottom(i);
        }
    };

    auto pop = [deque_size, eps](BoundedDeque<double>& deque){
        for(int i = deque_size - 1; i >= 0; --i){
            auto elem = deque.pop_bottom();
            EXPECT_TRUE( fabs(i) - fabs(elem) <= eps || elem == deque.Empty);
        }
    };

    auto steal = [deque_size, eps](BoundedDeque<double>& deque){
        for(int i = 0; i < deque_size; ++i){
            auto elem = deque.pop_top();
            EXPECT_TRUE(fabs(i) - fabs(elem) <= eps || elem == deque.Empty);
        }
    };

    auto pushPopSteal = [push, pop, steal](BoundedDeque<double>& deque1, BoundedDeque<double>& deque2, bool& guard){
        while(!guard){}; //sync label
        push(deque1);
        pop(deque1);
        steal(deque2);

    };


    bool gate = false;
    std::thread t1 = std::thread(pushPopSteal, std::ref(deque1), std::ref(deque2), std::ref(gate));
    std::thread t2 = std::thread(pushPopSteal, std::ref(deque2), std::ref(deque1), std::ref(gate));
    gate = true;

    t1.join();
    t2.join();

}

