#include "common.hpp"
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>

#include "CLI11.hpp"

std::string model = "hpx_forkjoin";
hpx::execution::experimental::fork_join_executor* fj = nullptr;

void single_run(unsigned num_threads) {
  hpx::execution::experimental::num_cores n_cores(num_threads);
  
  hpx::for_each(hpx::execution::par.on(*fj).with(n_cores), input_vec.begin(), input_vec.end(), [&](float &x){
      x = 42 * x;
  });
}

#include "hpx_main.hpp"

