#include "common.hpp"
#include <omp.h>
#include "CLI11.hpp"

std::string model = "omp";

void single_run(unsigned num_threads) {
  omp_set_num_threads(num_threads);
    #pragma omp parallel for
    for (int i=0; i<input_size; i++) {
        float x = input_vec[i];
        for (int j = 0; j < 100; j++)
        {
            x = std::sin(x) + std::cos(x);
        }
        input_vec[i] = x;
    }
}

#include "main.hpp"