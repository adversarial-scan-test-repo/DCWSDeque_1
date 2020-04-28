/**
 * AMP Project 11
 * DCWSDeque.h
 * Purpose: models a Dynamic Circular Work-Stealing Deque
 *
 * @author Sabrina Kall
 * @version 1.0 01.05.2018
 */

#ifndef PROJECT11_DCWSDEQUE_H
#define PROJECT11_DCWSDEQUE_H

#include <atomic>
#include <clocale>
#include "CircularArray.h"
#include "MemoryPool.h"
#include "../test/lib/googletest-master/googletest/include/gtest/gtest_prod.h"

template <class T>
class DCWSDeque {

private:

    /**
     *The top index indicates the topmost ("oldest") element in the deque (if there is any),
     * and is incremented on every steal operation. It is never decremented
     */
    std::atomic_long top{0};

    /**
     * Local value storing the last value of top so that top doesn't need to be called every time we want to check
     * if the array needs to be grown. Since top is never decremented, the real size (bottom -top)can only be
     * smaller than the upper bound (bottom - localTop). This value is updated at array expansion
     */
    long localTop = 0;

    /**
     * the bottom index indicates the next available slot in the array where the next new element is pushed,
     * and is incremented on every pushBottom operation
     *
     * If bottom is less than or equal to top, the deque is empty.
     *
     */
    std::atomic_long bottom{0};

    /**
     * Atomic cyclic array in which elements of type T are stored between indexes top and bottom
     */
    CircularArray<T>* activeArray;

    /**
     * Pointer to shared memory pool
     */
    MemoryPool<T>* memoryPool;

    /**
     * Atomically modifies top to the new value if and only if top is equal to the old value.
     * Implemented using the Compare-And-Exchange instruction
     *
     * @param oldVal
     * @param newVal
     * @return true if top has been modified, else false
     */
    bool casTop(long oldVal, long newVal);


    /**
     * Allows us to test private function casTop because we're paranoid
     */
    FRIEND_TEST(DCWSDequeTest, MandatoryCastopTest);
    FRIEND_TEST(DCWSDequeTest, DequeGrowsItsArrayAndShrinksItBack);
    FRIEND_TEST(DCWSDequeTest, DequeGrowsStealsShrinks);

public:

    /**
     * Element returned on an operation's attempt to remove an element from an empty deque
     * Used by pop() operations such as popBottom() and steal()
     */
    static constexpr T Empty = T();

    /**
     * Element returned on steal() if the operation loses a race with another operation (popBottom() or steal())
     * to remove an element from the deque
     */
    static constexpr T Abort = T();

    /**
     * Constructor
     *
     * @param memoryPool
     */
    DCWSDeque(MemoryPool<T>* memoryPool);


    /**
     * Pushes element onto the bottom of the deque
     *
     * Invoked only by the deque's owner
     *
     * @param element
     */
    void pushBottom(T element);


    /**
     * Pops an object from the bottom of the deque if the deque is not empty, otherwise returns Empty.
     *
     * Invoked only by the deque's owner
     *
     * @return the object from the bottom of the deque, or Empty
     */
    T popBottom();

    /**
     * If the deque is empty, returns Empty. Otherwise, returns the element successfully stolen from the top of
     * the deque, or returns Abort if this process loses a race with another process to steal the topmost element.
     *
     * @return an object from the top of the deque, Empty or Abort
     */
    T steal();

    /**
     * The perhapsShrink method shrinks the array if the number of elements in the deque is less than some fraction 1/K
     *  of the array size, where K ≥ 3.
     *
     * @param bottom
     * @param top
     */
    void perhapsShrink(long bottom, long top);

};


template  <class T>
DCWSDeque<T>::DCWSDeque(MemoryPool<T>* memoryPool){
    this->memoryPool = memoryPool;
    activeArray = new CircularArray<T>((int)log2(NB_ELEMENTS_IN_POOL_ELEM), this->memoryPool);
}


template <class T>
bool DCWSDeque<T>::casTop(long oldVal, long newVal) {
    return this->top.compare_exchange_weak(oldVal, newVal);
}


template <class T>
void DCWSDeque<T>::pushBottom(T element) {

    //get values
    long b = this->bottom;
    CircularArray<T>* a = this->activeArray;

    long size = (b - this->localTop);

    //if current array is full, enlarge it. For memory reclamation reasons, one cell is always left unused
    if(size >= a->arraySize() - 1) {
        long t = this->top;
        this->localTop = t;

        a = a->grow(b, t);
        this->activeArray = a;
    }

    //put the new element into the array at index bottom
    a->put(b, element);

    //increment bottom to show a new empty slot: abstract pushBottom takes place here
    bottom = (b + 1);

}

template <class T>
T DCWSDeque<T>::steal() {

    //get values
    long t = this->top;
    CircularArray<T> *oldArray = (this->activeArray);
    long b = this->bottom;
    CircularArray<T> *a = this->activeArray;

    long size = (b - t);

    //check if deque is empty
    if(size <= 0) {
        return this->Empty;
    }

    //check if deck has been resized while steal was happening -> if yes, Abort, other operations take precedence
    if((size% a->arraySize())== 0) {
        if(a->equals(oldArray) && t == this->top) {
            return this->Empty;
        }
        else {
            return this->Abort;
        }
    }

    //Read the top element
    T stolenElem = a->get(t);

    //Try to increment the index of top atomically. If this fails, it means another process has already taken the element
    if(!casTop(t, t+1)) {
        return  this->Abort;
    }

    return stolenElem;
}

template <class T>
T DCWSDeque<T>::popBottom() {

    //get values
    long b = this->bottom;
    CircularArray<T>* a = this->activeArray;

    //decrement bottom to the index of the last filled slot
    b-=1;
    this->bottom = b;

    long t = this->top;
    long size = (b - t);

    //if deque is empty ( < 0 after decrement of b), reset it to cannonical empty state t==b
    if(size < 0) {
        this->bottom = t;
        return this->Empty;
    }

    //get element out of array
    T poppedElem = a->get(b);

    //If there are still elements in the deque, check if it needs shrinking and return element
    if(size > 0) {
        perhapsShrink(b, t);
        return poppedElem;
    }

    //if we have taken the last element of the deque, we must perform a CAS on top to see if it won or lost any race
    // (with a concurrent steal operation) to pop the last item.
    if(!casTop(t, t+1)) {
        poppedElem = this->Empty;
    }

    //whether we have won or lost, we increment bottom to mark the deque empty
    // (we never decrement top, we increment bottom instead)
    this->bottom = t + 1;

    return poppedElem;

}

template <class T>
void DCWSDeque<T>::perhapsShrink(long bottom, long top) {

    //get values
    auto a = this->activeArray;
    long size = (bottom -top);

    //if the number of elements in the deque is less than some fraction 1/K of the array size, we reduce its size
    if(size < (a->arraySize()/a->shrinkConstant)) {
        auto aa = a->shrink(bottom, top);

        this->activeArray = aa;
        long ss = aa->arraySize();
        this->bottom = (bottom + ss);
        top = this->top;
        if(! casTop(top, top + ss)) {
            this->bottom = bottom;
        }
        a->free();//-> when we have a shared thread pool
    }
}


#endif //PROJECT11_DCWSDEQUE_H
