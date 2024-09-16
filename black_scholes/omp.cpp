#include "common.hpp"
#include <omp.h>
#include "CLI11.hpp"

void bs_omp_parallel_for(unsigned num_threads) {
  omp_set_num_threads(num_threads);
  for (int j=0; j<NUM_RUNS; j++) {
    #pragma omp parallel for
    for (int i=0; i<numOptions; i++) {

      /* Calling main function to calculate option value based on
       * Black & Scholes's equation.
       */
      float price = BlkSchlsEqEuroNoDiv( sptprice[i], strike[i],
          rate[i], volatility[i], otime[i],
          otype[i], 0);
      prices[i] = price;

    }
  }
}

std::chrono::microseconds measure_time_omp(unsigned num_threads) {
  auto beg = std::chrono::high_resolution_clock::now();
  bs_omp_parallel_for(num_threads);
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(end - beg);
}

double black_scholes(
  const int size,
  const unsigned num_threads,
  const unsigned num_rounds
) {
    generate_options(size);

    double runtime {0.0};

    for(unsigned j=0; j<num_rounds; ++j) {
      runtime += measure_time_omp(num_threads).count();
    }

    destroy_options();

    return (runtime / num_rounds) * 1e-6;
}

std::string model = "omp";

#include "main.hpp"