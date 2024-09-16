#include "common.hpp"
#include <tbb/global_control.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>

#include "CLI11.hpp"

void bs_tbb(unsigned num_threads) {

  tbb::global_control control(
    tbb::global_control::max_allowed_parallelism, num_threads
  );

	for (int j=0; j<NUM_RUNS; j++) {
    tbb::parallel_for(0, numOptions, 1, [&](int i){
      auto price = BlkSchlsEqEuroNoDiv(
        sptprice[i], strike[i],
        rate[i], volatility[i], otime[i],
        otype[i], 0
      );

      prices[i] = price;
#ifdef ERR_CHK
      check_error(i, price);
#endif
    });
	}
}

std::chrono::microseconds measure_time_tbb(unsigned num_threads) {
  auto beg = std::chrono::high_resolution_clock::now();
  bs_tbb(num_threads);
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
      runtime += measure_time_tbb(num_threads).count();
    }

    destroy_options();

    return (runtime / num_rounds) * 1e-6;
}

std::string model = "tbb";

#include "main.hpp"
