//
// Created by lu on 23.05.18.
//

#ifndef BENCHMARK2_BOUNDEDDEQUE_H
#define BENCHMARK2_BOUNDEDDEQUE_H

#include <atomic>
#include <cstddef>
#include <stdlib.h>

template <class T>
class BoundedDeque {
private:
    std::atomic_long bottom;
    std::atomic_long top;

    const long max_size;

    T* array;



public:
    /**
     * Static element, returned by pop operations if queue is empty
     */
    static constexpr T Empty = T(0);

    /** \brief Standard Constructor for array-based DEQUE
     * \param capacity size of array
     */
    BoundedDeque(int capacity);


    /** Pushes ITEM on the DEQUE
     * \param item element to be queued onto the DEQUE
     * \return TRUE if ITEM was sucessfully pushed into DEUQUE else FALSE
     */
    bool push_bottom(T item);

    /**
     * returns bottom most element of DEQUE
     * @return last enqueued element and removes it from the DEQUE
     */
    T pop_bottom();


    /**
     * \brief steals an element from the top of the queue. Only executed by "thiefs"
     * \return top most element and removes it from DEQUE
     */
    T pop_top();

    /** Deconstructor */
    ~BoundedDeque();



};

/// Implementation

template <class T>
BoundedDeque<T>::BoundedDeque(int capacity): bottom{0}, top{0}, max_size{capacity} {
    array = new T[capacity];
}


template <class T>
bool BoundedDeque<T>::push_bottom(T item) {
    long t = top;
    if( (bottom -t ) > (max_size - 1)) { //array is full
        return false;
    }
    array[bottom%max_size] = item;
    ++bottom;
    return true;
}

template <class T>
T BoundedDeque<T>::pop_bottom() {
    if(bottom == top){
        return this->Empty;
    }
    --bottom;
    auto val = array[bottom%max_size];

    long t = top;

    if(bottom > t) {
        return val;
    }
    if(bottom == t){
        if(top.compare_exchange_weak(t,t)){
            return  val;
        }
        bottom++; //restore
    }else{
        bottom = t;
    }
    return this->Empty;
}

template <class T>
T BoundedDeque<T>::pop_top() {
    long t = top;
    if(bottom <= t){
        return Empty;
    }
    T val = array[t%max_size];

    if(top.compare_exchange_weak(t, t+1)){
        return  val;
    } else{
        return Empty; //TODO is really empty or abort?
    }
}

template <class T>
BoundedDeque<T>::~BoundedDeque() {
    free(array);
}


#endif //BENCHMARK2_BOUNDEDDEQUE_H
