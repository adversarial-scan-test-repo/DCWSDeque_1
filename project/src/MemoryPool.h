//
// Created by sabrina on 09.05.18.
//

#ifndef PROJECT10_MEMORYPOOL_H
#define PROJECT10_MEMORYPOOL_H

#define MAX_POOL_SIZE  32 //16
#define NB_ELEMENTS_IN_POOL_ELEM 4 //72

#include <cstddef>
#include <atomic>

template <class T>
class MemoryPool {

private:
    //size_t* startOfFreeMemory;


    T* pool;

    std::atomic_size_t freeMemoryZoneIndicator[MAX_POOL_SIZE];

public:

    MemoryPool();

    //MemoryPool<T>* operator=( MemoryPool<T>* rhs);

    T* allocate(size_t memorySizeNeeded);

    void free(T* memoryToFree);

};

template<class T>
MemoryPool<T>::MemoryPool() {
    //claim memory from heap
    pool = new T[MAX_POOL_SIZE * NB_ELEMENTS_IN_POOL_ELEM];
    //mark whole array as free
    for(int i = 0; i < MAX_POOL_SIZE; ++i){
        freeMemoryZoneIndicator[i] = 0;
    }

}

/*
template<class T>
MemoryPool<T>* MemoryPool<T>::operator=(MemoryPool<T>* rhs){
    //pool = rhs.pool;

}*/

template<class T>
T* MemoryPool<T>::allocate(size_t memorySizeNeeded) {
    size_t unused = 0; //needed for CAS operation

    int i;
    for(i = 0; i < MAX_POOL_SIZE; i+= freeMemoryZoneIndicator[i]) {
        if((freeMemoryZoneIndicator[i]).compare_exchange_weak(unused, memorySizeNeeded)) {
            bool allFree = true;
            for(int j = 1; j < memorySizeNeeded && allFree; ++j) {
                if(freeMemoryZoneIndicator[i + j] != 0) {
                    freeMemoryZoneIndicator[i] = 0;
                    allFree = false;
                    i+=j;
                }
            }

            if(allFree) {
                return &pool[i*NB_ELEMENTS_IN_POOL_ELEM];
            }
        }
    }

    return nullptr;
}

template<class T>
void MemoryPool<T>::free(T *memoryToFree) {
    size_t i = (memoryToFree - pool)/NB_ELEMENTS_IN_POOL_ELEM;
    ASSERT_LT(i, MAX_POOL_SIZE);
    freeMemoryZoneIndicator[i] = 0;

}


#endif //PROJECT10_MEMORYPOOL_H
