#include "common.hpp"
#include <omp.h>
#include "CLI11.hpp"

std::string model = "omp";

void single_run(unsigned num_threads) {
  omp_set_num_threads(num_threads);
    #pragma omp parallel for
    for (int i=0; i<input_size; i++) {
        float x = input_vec[i];
        x = 42 * x;
        input_vec[i] = x;
    }
}

#include "main.hpp"