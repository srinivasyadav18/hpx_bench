#include "common.hpp"
#include <taskflow/taskflow.hpp>
#include <taskflow/algorithm/for_each.hpp>
#include <taskflow/algorithm/scan.hpp>
#include "CLI11.hpp"

std::string model = "taskflow";

void single_run(unsigned num_threads) {
  tf::Executor executor(num_threads);
  tf::Taskflow taskflow;

  float result;

  taskflow.inclusive_scan(input_vec.begin(), input_vec.end(), output_vec.begin(), [](float l, float r){
    return l + r;
  });

  executor.run(taskflow).get();
}

#include "main.hpp"
