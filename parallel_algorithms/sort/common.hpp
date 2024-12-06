#include <cstdlib>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>

std::vector<float> input_vec;
int input_size;

std::vector<int> threads{1, 2, 4, 8, 16, 32};
std::vector<int> sizes{ 100'000, 10'000'000, 1'000'000'000};
unsigned num_rounds {1};

float result{};

struct gen_float_t{
    std::mt19937 mersenne_engine {42};
    std::uniform_real_distribution<float> dist_float {1, 1024};
    inline auto operator()()
    {
        return dist_float(mersenne_engine);
    }
} gen_float{};

inline void generate_vec(int size)
{
    input_size = size;
    input_vec.resize(input_size);
    std::generate(input_vec.begin(), input_vec.end(), gen_float);
}

inline void destroy_vec()
{
    input_vec.clear();
}
