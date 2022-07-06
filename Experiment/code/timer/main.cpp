#include "process.hpp"

// Function: measure_time_taskflow
std::chrono::milliseconds measure_time_taskflow( unsigned num_threads, unsigned num_run=1) {
  auto t1 = std::chrono::high_resolution_clock::now();
  run_taskflow(num_threads, num_run);
  auto t2 = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
}

// Function: measure_time_tbb
std::chrono::milliseconds measure_time_tbb( unsigned num_threads, unsigned num_run=1) {
  auto t1 = std::chrono::high_resolution_clock::now();
  run_tbb(num_threads, num_run);
  auto t2 = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
}

int main(int argc, char* argv[]) { 

  unsigned num_threads = std::thread::hardware_concurrency();
  if(argc > 1) {
    num_threads = std::atoi(argv[1]);
  }
  std::cout << "num threads = " << num_threads << std::endl;
 
  int rounds {10};

  std::cout << std::setw(12) << "TBB"
            << std::setw(12) << "Taskflow"
            << std::setw(12) << "speedup"
            << '\n';
    
  double tbb_time {0.0};
  double tf_time  {0.0};

  for(int j=0; j<rounds; ++j) {
    std::puts("TBB starts");
    tbb_time += measure_time_tbb(num_threads, 1).count();
    //std::puts("Taskflow starts");
    //tf_time  += measure_time_taskflow(num_threads, 1).count();
  }

  std::cout << std::setw(12) << tbb_time / rounds / 1e3 
            << std::setw(12) << tf_time  / rounds / 1e3 
            << std::setw(12) << tbb_time / tf_time
            << std::endl;

  return EXIT_SUCCESS; 
}


