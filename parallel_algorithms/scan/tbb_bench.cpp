#include "common.hpp"
#include <tbb/global_control.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_scan.h>

#include "CLI11.hpp"

std::string model = "tbb";

void single_run(unsigned num_threads) {

    tbb::global_control control(
        tbb::global_control::max_allowed_parallelism, num_threads
    );

	using range_type = tbb::blocked_range<size_t>;
	float sum = tbb::parallel_scan(range_type(0, input_size), 0,
		[&](const range_type &r, float sum, bool is_final_scan) {
			float tmp = sum;
			for (size_t i = r.begin(); i < r.end(); ++i) {
				tmp += input_vec[i];
				if (is_final_scan) {
					output_vec[i] = tmp;
				}
			}
			return tmp;
		},
		[&](const float &a, const float &b) {
			return a + b;
		});
    result += sum;
}

#include "main.hpp"