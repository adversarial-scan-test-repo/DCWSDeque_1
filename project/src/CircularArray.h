/**
 * AMP Project 10
 *
 * CircularArray.h
 *
 * Purpose: models a circular resizeable array to store elements of type T
 *
 * @author Sabrina Kall
 * @version 1.0 01.05.2018
 */

#ifndef PROJECT11_CIRCULARARRAY_H
#define PROJECT11_CIRCULARARRAY_H

#include <algorithm>
#include "MemoryPool.h"


/**
 * Class modelling a circular array of elements of type T
 *
 * Note that elements are indexed modulo the array size.
 *
 * @tparam T
 */
template <class T>
class CircularArray {

private:
    /**
     * binary logarithm of the size of the array
     * (easier to double size)
     */
    int logSize;

    /**
     * Array keeping the elements of the circular array
     */
    T* segment;

    MemoryPool<T> *memoryPool;


    /**
     * Reference to a smaller array from which the present array was grown (if exists)
     * This prevents the garbage collector from reclaimîng the space, and since the array keeps its
     * elements, we only have to replace the ones that have changed
     *
     */
    CircularArray<T>* smallerArray;

    /**
     * Flag indicating if the current array was grown from a smaller array
     * (Existence of smallerArray)
     */
    bool smallest = true;

    /**
     * Indicates the lowest value of bottom in with an element was stored while this array was active
     * When the deque shrinks its array, only the elements stored in indexes greater than or equal to the
     * low watermark of the bigger array are copied. (The smaller array's watermark is updated to the min of
     * the two watermarks)
     */
    long watermark = 0;


public:

    /**
     * Constructor
     *
     * @param logSize
     */
    explicit CircularArray(int logSize, MemoryPool<T>* memoryPool1);

    /**
     *  = operator for T=int class
     */
    CircularArray<T>& operator=(const CircularArray<T>& rhs);

    /**
         * Returns the current size of the array
         * @return
         */
    long arraySize();

    /**
     * Gets the element stored at index i in the array
     * @param i
     * @return
     */
    T get(long i);

    /**
     * Puts element at index i (modulo size) in the array
     *
     * @param i
     * @param element
     */
    void put(long i, T element);

    /**
     * Grows the array to double its current size
     *
     * @param bottom largest index where an element is stored
     * @param top smallest index where an element is stored
     * @return the bigger array
     */
    CircularArray<T>* grow(long bottom, long top);


    /**
     * Shrinks the array to half its current size
     *
     * @param bottom largest index where element is stored in original array
     * @param top smallest index where an element is stored in original array
     * @return the smaller array
     */
    CircularArray<T>* shrink(long bottom, long top);

    /**
     * Shrinks the array if the number of elements in the deque is less than some fraction
     * 1/K of the array size, where K ≥ 3.
     *
     */
    const int shrinkConstant;

    /**
     * Checks two arrays for equality
     *
     * @param thisArray
     * @param thatArray
     * @return
     */
    bool equals(CircularArray<T> *thatArray);

    void free();


};

//======================================================================================================================
//Implementation (template classes cannot be separated into header and cpp files)
//======================================================================================================================


template<class T>
CircularArray<T>::CircularArray(int logSize, MemoryPool<T>* memoryPool1):logSize(logSize), shrinkConstant(3){
    smallerArray = nullptr;
    this->memoryPool = memoryPool1;
    this->segment = memoryPool->allocate(1<<this->logSize);

}

template<class T>
CircularArray<T>& CircularArray<T>::operator=(const CircularArray<T> &rhs) {
    //expects that this was already created.
    logSize = rhs.logSize;
    segment = rhs.segment;
    smallerArray = rhs.smallerArray;
    smallest = rhs.smallest;
    watermark = rhs.watermark;
    memoryPool = rhs.memoryPool;

    return *this;
}

template <class T>
long CircularArray<T>::arraySize() {
    return 1<<(this->logSize);
}

template <class T>
T CircularArray<T>::get(long i) {
    return this->segment[i%(this->arraySize())];
}

template <class T>
void CircularArray<T>::put(long i, T element) {
    this->segment[i%this->arraySize()] = element;
}

template <class T>
CircularArray<T>* CircularArray<T>::grow(long bottom, long top) {

    //Double size of array by adding 1 to the log
    auto a =  new CircularArray<T>(this->logSize + 1, this->memoryPool);

    a->smallerArray = this;
    a->smallest = false;
    a->watermark = bottom;

    //Copy over the elements
    for (long i = top; i < bottom; i++) {
        a->put(i, this->get(i));
    }


    return a;
}

template<class T>
CircularArray<T>* CircularArray<T>::shrink(long bottom, long top) {

    //Size is minimal, shrink impossible: we just return this
    if(this->arraySize() <= 1) {
        return this;
    }

    CircularArray<T>* a;

    //If this array does not reference a smaller array, create a new one of half the size
    if(this->smallest) {
        a = new CircularArray(this->logSize - 1, this->memoryPool);

    } else {

        //Reference to a smaller array exists: use it to find the best fit among referenced arrays
        long sizeNeeded = (bottom - top);
        long minWatermark = this->watermark;

        CircularArray<T>* levelDownArray = this;

        //Look through the nested references to smaller arrays to find one of best sizeNeeded
        while(!levelDownArray->smallest && sizeNeeded < levelDownArray->smallerArray->arraySize()) {

            levelDownArray = levelDownArray->smallerArray;

            //we are looking for the minimum watermark among the nested array references: this is the top from which
            //elements will need to be copied in in case changes occurred
            if(levelDownArray->watermark < minWatermark) {
                minWatermark = levelDownArray->watermark;

            }
        }

        a = levelDownArray;

        //we only need to copy over elements stored in indices higher that the watermark, the others are the same
        top = minWatermark;
        a->watermark = minWatermark;
    }

    //copy over the elements
    for (long i = top; i < bottom; i++) {
        a->put(i, this->get(i));
    }

    return a;
}


template <class T>
bool CircularArray<T>::equals(CircularArray<T> *thatArray) {
    long size = this->arraySize();

    if(size != thatArray->arraySize()) {
        return false;
    } else {
        for(int i = 0; i < size; i++) {
            if(this->get(i) != thatArray->get(i)) {
                return false;
            }
        }
    }
    return true;
}


template <class T>
inline void CircularArray<T>::free(){
    memoryPool->free(segment);
}




#endif //PROJECT11_CIRCULARARRAY_H