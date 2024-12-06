#include "common.hpp"
#include <tbb/global_control.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_sort.h>

#include "CLI11.hpp"

std::string model = "tbb";

void single_run(unsigned num_threads) {

    tbb::global_control control(
        tbb::global_control::max_allowed_parallelism, num_threads
    );

  	tbb::parallel_sort(input_vec.begin(), input_vec.end());
}

#include "main.hpp"