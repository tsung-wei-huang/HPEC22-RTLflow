#include "process.hpp"
#include <taskflow/taskflow.hpp>  

struct ProcessFramework : public tf::Framework {
  ProcessFramework(int start, int num,
    const std::experimental::filesystem::path& path, std::array<std::vector<float>, NUM_TIMERS> &slacks) {
    auto file_task = emplace([id=start, &slacks](){ read_slack(id, slacks); });
    for(int i=0; i<num; i++) {
      emplace([&, id = start+i] () {   
        auto args = create_args(id, path);
        execute_process(path, args);
      }).precede(file_task);
    }
  }
};  // End of ProcessFramework

struct SimulationFramework : public tf::Framework {
  SimulationFramework(std::vector<std::unique_ptr<ProcessFramework>>& process_frameworks, const int num_process) {
    for(int i=0; i<num_process; i++) {
      composed_of(*process_frameworks[i]);
    }
  }
}; // End of SimulationFramework

struct ScenarioFramework: public tf::Framework {
  ScenarioFramework(std::array<std::vector<float>, NUM_TIMERS>& slacks, 
    std::array<std::vector<float>, NUM_TIMERS>& coefficients) {
    for(size_t i=0; i<slacks.size(); i++) {
      emplace([&, i=i, NUM_TIMERS=slacks.size()] () {
        for(size_t j=0; j<NUM_TIMERS; j++) { 
          if(i != j) {
            coefficients[i].push_back(correlation_coefficient(slacks[i], slacks[j]));     
          }
          else {
            coefficients[i].push_back(0); 
          }
        }
      });
    }
  }
};  // End of ScenarioFramework


void run_taskflow(unsigned num_threads, unsigned num_run){

  tf::Executor executor(num_threads);

  //tf::Taskflow tf(num_threads);
  tf::Taskflow tf;
  std::array<std::vector<float>, NUM_TIMERS> slacks;
  std::array<std::vector<float>, NUM_TIMERS> coefficients;

  constexpr int num_process {NUM_TIMERS/BATCH_SIZE};
  const std::experimental::filesystem::path binary_path {"/home/clin99/Software/OpenTimer/SC19/timer"};

  std::vector<std::unique_ptr<ProcessFramework>> process_frameworks;
  for(int i=0; i<num_process; i++) {
    process_frameworks.emplace_back(std::make_unique<ProcessFramework>(i*BATCH_SIZE, BATCH_SIZE, binary_path, slacks));
  }

  SimulationFramework simulations(process_frameworks, num_process);
  ScenarioFramework scenarios(slacks, coefficients);

  tf::Framework analysis;
  analysis.composed_of(simulations).precede(analysis.composed_of(scenarios));

  //analysis.dump(std::cout);
  //exit(0);

  for(size_t r=0; r<num_run; r++) {
    tf.run_n(analysis, 1).get();
    for(size_t i=0; i<NUM_TIMERS; i++) {
      slacks[i].clear();
      coefficients[i].clear();
    }
  }
}

