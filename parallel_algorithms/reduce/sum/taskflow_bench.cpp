#include "common.hpp"
#include <taskflow/taskflow.hpp>
#include <taskflow/algorithm/for_each.hpp>
#include <taskflow/algorithm/reduce.hpp>
#include "CLI11.hpp"

std::string model = "taskflow";

void single_run(unsigned num_threads) {
  tf::Executor executor(num_threads);
  tf::Taskflow taskflow;

  float result;

  taskflow.reduce(input_vec.begin(), input_vec.end(), result, [](float l, float r){
    return l + r;
  });

  executor.run(taskflow).get();
}

#include "main.hpp"
