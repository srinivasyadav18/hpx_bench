#include "common.hpp"
#include <execution>
#include <algorithm>

#include "CLI11.hpp"

std::string model = "std";

void single_run(unsigned num_threads) {
    std::for_each(std::execution::par, input_vec.begin(), input_vec.end(), [&](float &x){
        x = 42 * x;
    });
}

#include "main.hpp"
