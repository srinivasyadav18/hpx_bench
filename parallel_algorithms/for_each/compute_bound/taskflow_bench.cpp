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
        for (int j = 0; j < 100; j++)
        {
            x = std::sin(x) + std::cos(x);
        }
    }, tf::StaticPartitioner()
  );

  executor.run(taskflow).get();
}

#include "main.hpp"
