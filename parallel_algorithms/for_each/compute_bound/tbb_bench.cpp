#include "common.hpp"
#include <tbb/global_control.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>

#include "CLI11.hpp"

std::string model = "tbb";

void single_run(unsigned num_threads) {

    tbb::global_control control(
        tbb::global_control::max_allowed_parallelism, num_threads
    );

    tbb::parallel_for(0, input_size, 1, [&](int i){
        float x = input_vec[i];
        for (int j = 0; j < 100; j++)
        {
            x = std::sin(x) + std::cos(x);
        }
        input_vec[i] = x;
        });
}

#include "main.hpp"