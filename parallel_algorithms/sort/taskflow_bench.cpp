#include "common.hpp"
#include <taskflow/taskflow.hpp>
#include <taskflow/algorithm/for_each.hpp>
#include <taskflow/algorithm/sort.hpp>
#include "CLI11.hpp"

std::string model = "taskflow";

void single_run(unsigned num_threads) {
  tf::Executor executor(num_threads);
  tf::Taskflow taskflow;

  taskflow.sort(input_vec.begin(), input_vec.end());

  executor.run(taskflow).get();
}

#include "main.hpp"
