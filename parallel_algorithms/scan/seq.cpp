#include "common.hpp"
#include <execution>
#include <algorithm>

#include "CLI11.hpp"

std::string model = "seq";

void single_run(unsigned num_threads) {
    std::inclusive_scan(std::execution::seq, input_vec.begin(), input_vec.end(), output_vec.begin());
}

#include "main.hpp"