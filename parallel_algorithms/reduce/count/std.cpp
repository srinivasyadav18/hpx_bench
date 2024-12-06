#include "common.hpp"
#include <execution>
#include <algorithm>

#include "CLI11.hpp"

std::string model = "std";

void single_run(unsigned num_threads) {
    result += std::reduce(std::execution::par, input_vec.begin(), input_vec.end());
}

#include "main.hpp"
