


std::chrono::microseconds measure_time(unsigned num_threads) {
  auto beg = std::chrono::high_resolution_clock::now();
  single_run(num_threads);
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(end - beg);
}

double run(
  const int size,
  const unsigned num_threads,
  const unsigned num_rounds
) {
    double runtime {0.0};
    for(unsigned j=0; j<num_rounds; ++j) {
      generate_vec(size);
      runtime += measure_time(num_threads).count();
      destroy_vec();
    }
    return (runtime / num_rounds) * 1e-6;
}

int hpx_main()
{
  if (model == "hpx_forkjoin")
    fj = new hpx::execution::experimental::fork_join_executor{};

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
      auto rt = run(size, thread, num_rounds);
      csv_writer << size << "," << thread << "," << rt << "\n";
      std::cout << std::setw(12) << size
          << std::setw(12) << thread
          << std::setw(24) << rt
          << std::endl;
    }
  }
  if (model == "hpx_forkjoin")
    delete fj;

  return hpx::finalize();
}

int main (int argc, char *argv[]) {
  CLI::App app{"Option pricing with Black-Scholes Partial Differential Equation"};
  app.add_option("-t,--num_threads", threads, "number of threads (default={1})");
  app.add_option("-s,--sizes", sizes, "problem sizes (default={1000})");
  app.add_option("-r,--num_rounds", num_rounds, "number of rounds (default=1)");

  CLI11_PARSE(app, argc, argv);

  return hpx::init();
}
