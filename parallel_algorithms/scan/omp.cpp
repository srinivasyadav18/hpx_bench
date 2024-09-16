#include "common.hpp"
#include <omp.h>
#include "CLI11.hpp"

std::string model = "omp";

void single_run(unsigned num_threads) {
  omp_set_num_threads(num_threads);
  float local_result{};
    #pragma omp parallel for reduction (+:local_result)
    for (int i=0; i<input_size; i++) {
        local_result += input_vec[i];
    }
  result += local_result;
}

#include "main.hpp"