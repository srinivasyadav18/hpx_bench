#include "common.hpp"
#include <tbb/global_control.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>

#include "CLI11.hpp"

std::string model = "tbb";

void single_run(unsigned num_threads) {

    tbb::global_control control(
        tbb::global_control::max_allowed_parallelism, num_threads
    );

    auto total = tbb::parallel_reduce( 
                  tbb::blocked_range<int>(0,input_size),
                  0.0,
                  [&](tbb::blocked_range<int> r, float running_total)
    {
        for (int i=r.begin(); i<r.end(); ++i)
        {
            running_total += input_vec[i];
        }

        return running_total;
    }, std::plus<float>());
}

#include "main.hpp"