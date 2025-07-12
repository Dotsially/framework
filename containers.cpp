#ifndef CONTAINER
#define CONTAINER
#include "common.cpp"


template <typename T>
struct SwapbackArray{
    T* data;
    uint32_t size = 0;
    uint32_t capacity = 0;
};

template <typename T>
void SwapbackInitialize(SwapbackArray<T>* array){
    T* allocatedData = (T*)calloc(4, sizeof(T));
    array->data = allocatedData;
    array->capacity = 4;
}

template <typename T>
void SwapbackInsert(SwapbackArray<T>* array, T object){
    if(array->size >= array->capacity){
        T* allocatedData = (T*)calloc(array->capacity*2, sizeof(T));
        
        for (int i = 0; i < array->size; i++){
            allocatedData[i] = array->data[i];
        }
        
        free(array->data);

        array->data = allocatedData;
        array->capacity *= 2; 
    }

    array->data[array->size] = object;
    array->size += 1;
}

template <typename T>
void SwapbackRemove(SwapbackArray<T>* array, uint32_t index){
    uint32_t size = array->size;
    if (!(index >= 0 && index < size)){
        return;
    }

    T* data = array->data;

    data[index] = data[size-1];
    data[size-1] = {};
    array->size -= 1;
}

template <typename T>
void SwapbackClear(SwapbackArray<T>* array){
    T* data = array->data;
    for(int i = 0; i < array->size; i++){   
        data[i] = {};
    }

    array->size = 0;
}





#endif