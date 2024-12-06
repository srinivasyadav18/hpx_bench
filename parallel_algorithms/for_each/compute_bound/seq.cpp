#include "common.hpp"
#include <execution>
#include <algorithm>

#include "CLI11.hpp"

std::string model = "seq";

void single_run(unsigned num_threads) {
    std::for_each(std::execution::seq, input_vec.begin(), input_vec.end(), [&](float &x){
        for (int j = 0; j < 100; j++)
        {
            x = std::sin(x) + std::cos(x);
        }
    });
}

#include "main.hpp"