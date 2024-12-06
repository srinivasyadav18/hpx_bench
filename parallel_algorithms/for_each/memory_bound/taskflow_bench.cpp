#include "common.hpp"
#include <taskflow/taskflow.hpp>
#include <taskflow/algorithm/for_each.hpp>
#include "CLI11.hpp"

std::string model = "taskflow";

void single_run(unsigned num_threads) {
  tf::Executor executor(num_threads);
  tf::Taskflow taskflow;

  taskflow.for_each(
    input_vec.begin(), input_vec.end(), [](float& x){
        x = 42 * x;
    }, tf::StaticPartitioner()
  );

  executor.run(taskflow).get();
}

#include "main.hpp"
