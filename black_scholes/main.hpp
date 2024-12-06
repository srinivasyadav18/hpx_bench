

int main (int argc, char *argv[]) {

  CLI::App app{"Option pricing with Black-Scholes Partial Differential Equation"};

  std::vector<int> threads{1};
  app.add_option("-t,--num_threads", threads, "number of threads (default={1})");

  std::vector<int> sizes{1000};
  app.add_option("-s,--sizes", sizes, "problem sizes (default={1000})");

  unsigned num_rounds {3};
  app.add_option("-r,--num_rounds", num_rounds, "number of rounds (default=1)");

  CLI11_PARSE(app, argc, argv);

  std::cout << "model = " << model << "\n";
  std::cout << "threads = [ ";
  for (auto i : threads) std::cout << i << ", ";
  std::cout << "]\n";

  std::cout << "sizes = [ ";
  for (auto i : sizes) std::cout << i << ", ";
  std::cout << "]\n";

  std::ofstream csv_writer(model + ".csv");
  csv_writer << "threads,time\n";

  std::cout << "*********************************\n";
  std::cout << std::setw(12) << "size"
      << std::setw(12) << "threads"
      << std::setw(24) << "runtime"
      << std::endl;
  for (int size : sizes) {
    for (int thread : threads)
    {
      auto rt = black_scholes(size, thread, num_rounds);
      csv_writer << thread << "," << rt << "\n";

      std::cout << std::setw(12) << size
          << std::setw(12) << thread
          << std::setw(24) << rt
          << std::endl;
    }
  }
  return 0;
}