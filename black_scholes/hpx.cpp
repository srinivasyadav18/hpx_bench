#include "common.hpp"
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>

#include "CLI11.hpp"

void bs_hpx(unsigned num_threads) {
  hpx::execution::experimental::num_cores n_cores(num_threads);

	for (int j=0; j<NUM_RUNS; j++) {
    hpx::experimental::for_loop(hpx::execution::par.with(n_cores), 0, numOptions, [&](int i){
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

std::chrono::microseconds measure_time_hpx(unsigned num_threads) {
  auto beg = std::chrono::high_resolution_clock::now();
  bs_hpx(num_threads);
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(end - beg);
}


double black_scholes(
  const int size,
  const unsigned num_threads,
  const unsigned num_rounds
) {
    // hpx::post()
    generate_options(size);

    double runtime {0.0};

    for(unsigned j=0; j<num_rounds; ++j) {
      runtime += measure_time_hpx(num_threads).count();
    }

    destroy_options();
    return (runtime / num_rounds) * 1e-6;
}

std::string model = "hpx";

std::vector<int> threads{1};
std::vector<int> sizes{1000};
unsigned num_rounds {3};

int hpx_main()
{
  std::cout << "model = " << model << "\n";
  std::cout << "threads = [ ";
  for (auto i : threads) std::cout << i << ", ";
  std::cout << "]\n";

  std::ofstream csv_writer("hpx.csv");
  csv_writer << "threads,time\n";
  std::cout << "sizes = [ ";
  for (auto i : sizes) std::cout << i << ", ";
  std::cout << "]\n";

  std::cout << "*********************************\n";
  std::cout << std::setw(12) << "size"
      << std::setw(12) << "threads"
      << std::setw(24) << "runtime"
      << std::endl;
  for (int size : sizes) {
    for (int thread : threads)
    {
      auto rt = black_scholes(size, thread, num_rounds);
      csv_writer << thread << "," << rt << "\n";
      std::cout << std::setw(12) << size
          << std::setw(12) << thread
          << std::setw(24) << rt
          << std::endl;
    }
  }
  return hpx::finalize();
}

int main (int argc, char *argv[]) {

  CLI::App app{"Option pricing with Black-Scholes Partial Differential Equation"};
  app.add_option("-t,--num_threads", threads, "number of threads (default={1})");
  app.add_option("-s,--sizes", sizes, "problem sizes (default={1000})");
  app.add_option("-r,--num_rounds", num_rounds, "number of rounds (default=1)");

  CLI11_PARSE(app, argc, argv);

  return hpx::init();
}
