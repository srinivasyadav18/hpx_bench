#include "common.hpp"
#include <hpx/hpx.hpp>
#include <hpx/hpx_init.hpp>

#include "CLI11.hpp"

std::string model = "hpx_forkjoin";
hpx::execution::experimental::fork_join_executor* fj = nullptr;

void single_run() {
  hpx::for_each(hpx::execution::par.on(*fj), input_vec.begin(), input_vec.end(), [&](float &x){
      for (int j = 0; j < 100; j++)
      {
          x = std::sin(x) + std::cos(x);
      }
  });
}

double time_took{};

std::chrono::microseconds measure_time() {
  auto beg = std::chrono::high_resolution_clock::now();
  single_run();
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(end - beg);
}

int global_size{};
int hpx_main()
{
    fj = new hpx::execution::experimental::fork_join_executor{};
    // std::cout << "running hpx main " << std::endl;
    // std::cout << "hpx threads : " << hpx::get_num_worker_threads() << std::endl;

    double runtime {0.0};
    for(unsigned j=0; j<num_rounds; ++j) {
      generate_vec(global_size);
      runtime += measure_time().count();
      destroy_vec();
    }
    time_took = (runtime / num_rounds) * 1e-6;
    delete fj;
    return hpx::finalize();
}

double run(
  const int size,
  const unsigned num_threads
) {
    std::string thread_str = std::string("--hpx:threads=") + std::to_string(num_threads);
    // std::string thread_str = std::string("hpx.=4"); 
    // std::cout << "thread str : " << thread_str << std::endl;
    std::vector<std::string> const cfg = { thread_str};

    hpx::init_params init_args;
    init_args.cfg = cfg;
    global_size = size;

    // std::cout << "hpx init and run : " << size << "\t" << num_threads << std::endl;
    hpx::init(init_args);

    return time_took;
}

void benchmark()
{
  std::cout << "model = " << model << "\n";
  std::cout << "threads = [ ";
  for (auto i : threads) std::cout << i << ", ";
  std::cout << "]\n";

  std::ofstream csv_writer(model + ".csv");
  csv_writer << "size,threads,time\n";
  std::cout << "sizes = [ ";
  for (auto i : sizes) std::cout << i << ", ";
  std::cout << "]\n";

  std::cout << "*********************************\n";
  std::cout << std::setw(12) << "size"
      << std::setw(12) << "threads"
      << std::setw(24) << "runtime"
      << std::endl;
  for (int size : sizes) {
    for (int thread : threads)
    {
      auto rt = run(size, thread);
      csv_writer << size << "," << thread << "," << rt << "\n";
      std::cout << std::setw(12) << size
          << std::setw(12) << thread
          << std::setw(24) << rt
          << std::endl;
    }
  }
}

int main (int argc, char *argv[]) {
  CLI::App app{"Option pricing with Black-Scholes Partial Differential Equation"};
  app.add_option("-t,--num_threads", threads, "number of threads (default={1})");
  app.add_option("-s,--sizes", sizes, "problem sizes (default={1000})");
  app.add_option("-r,--num_rounds", num_rounds, "number of rounds (default=1)");

  CLI11_PARSE(app, argc, argv);

  benchmark();
  return 0;
}
