#ifndef FRAMEWORK_HELPER
#define FRAMEWORK_HELPER

#include "common.cpp"

template<typename T>
void VectorSwapbackRemoval(std::vector<T>* vector, size_t removalIndex){
    if (removalIndex < vector->size()) {
        (*vector)[removalIndex] = vector->back();
        vector->pop_back();
    }
}


#endif