#include "common.hpp"
#include <omp.h>
#include "CLI11.hpp"

std::string model = "omp";

template <typename V>
void mergeSortRecursive(V& v, size_t left, size_t right) {
  if (left < right) {
    if (right-left >= 32) {
      size_t mid = (left+right)/2;
      #pragma omp taskgroup
      {
        #pragma omp task shared(v) untied if(right-left >= (1<<14))
        mergeSortRecursive(v, left, mid);
        #pragma omp task shared(v) untied if(right-left >= (1<<14))
        mergeSortRecursive(v, mid+1, right);
        #pragma omp taskyield
      }
      std::inplace_merge(v.begin()+left, v.begin()+mid+1, v.begin()+right+1);
    } 
    else {
      std::sort(v.begin()+left, v.begin()+right+1);
    }
  }
}

void single_run(unsigned nthreads) {
  #pragma omp parallel num_threads(nthreads)
  {
    #pragma omp single
    mergeSortRecursive(input_vec, 0, input_vec.size()-1);
  }
}

#include "main.hpp"