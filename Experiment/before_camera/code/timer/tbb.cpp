#include "process.hpp"
#include <tbb/task_scheduler_init.h>
#include <tbb/flow_graph.h>

using namespace tbb;
using namespace tbb::flow;

struct ProcessGraph {
  ProcessGraph(int start, int num, 
    const std::experimental::filesystem::path& binary_path, 
    std::array<std::vector<float>, NUM_TIMERS>& slacks) {

    file_node =std::make_unique<continue_node<continue_msg>>(g, 
      [&, id=start](const continue_msg&) { 
        read_slack(id, slacks);
      }
    );

    for(int i=0; i<num ; i++) {
      nodes.emplace_back(
        std::make_unique<continue_node<continue_msg>>(g, 
          [&, id=start+i](const continue_msg&) { 
            auto args = create_args(id, binary_path);
            execute_process(binary_path, args);
          }
        )
      );
      make_edge(*nodes.back(), *file_node);
    }
  }

  void wait_for_all() {
    for(auto &n: nodes) {
      n->try_put(continue_msg());
    }
    g.wait_for_all();
  }

  tbb::flow::graph g;
  std::vector<std::unique_ptr<continue_node<continue_msg>>> nodes;
  std::unique_ptr<continue_node<continue_msg>> file_node;
};  // End of ProcessGraph

struct SimulationGraph {
  SimulationGraph(std::vector<std::unique_ptr<ProcessGraph>> &process_graphs) {
    for(auto &pg: process_graphs) {
      nodes.emplace_back(
        std::make_unique<continue_node<continue_msg>>(g, 
          [&](const continue_msg&) { 
            pg->wait_for_all();
          }
        )
      );
    }
  }

  void wait_for_all() {
    for(auto &n: nodes) {
      n->try_put(continue_msg());
    }
    g.wait_for_all();
  }

  tbb::flow::graph g; 
  std::vector<std::unique_ptr<continue_node<continue_msg>>> nodes;
}; // End of SimulationGraph

struct ScenarioGraph {
  ScenarioGraph(std::array<std::vector<float>, NUM_TIMERS>& slacks, 
    std::array<std::vector<float>, NUM_TIMERS>& coefficients) {
    for(size_t i=0; i<slacks.size(); i++) {
      nodes.emplace_back(std::make_unique<continue_node<continue_msg>>(g, 
        [&, num_timers=slacks.size(), i=i](const continue_msg&){
          for(size_t j=0; j<num_timers; j++) { 
            if(i != j) {
              coefficients[i].push_back(correlation_coefficient(slacks[i], slacks[j]));
            }
            else {
              coefficients[i].push_back(0); 
            }
          }
      }));
    }
  }

  void wait_for_all() {
    for(auto &n: nodes) {
      n->try_put(continue_msg());
    }
    g.wait_for_all();
  }
  tbb::flow::graph g; 
  std::vector<std::unique_ptr<continue_node<continue_msg>>> nodes;
}; // End of ScenarioGraph


void run_tbb(unsigned num_threads, unsigned num_run){

  tbb::task_scheduler_init init(num_threads);

  std::array<std::vector<float>, NUM_TIMERS> slacks;
  std::array<std::vector<float>, NUM_TIMERS> coefficients;

  constexpr int num_process {NUM_TIMERS/BATCH_SIZE};
  const std::experimental::filesystem::path binary_path {"/home/clin99/Software/OpenTimer/SC19/timer"};
 
  std::vector<std::unique_ptr<ProcessGraph>> process_graphs;
  for(int i=0; i<num_process; i++) {
    process_graphs.emplace_back(std::make_unique<ProcessGraph>(i*BATCH_SIZE, BATCH_SIZE, binary_path, slacks));
  }

  SimulationGraph simulation_graph(process_graphs);
  ScenarioGraph scenario_graph(slacks, coefficients);

  tbb::flow::graph analysis_graph;
  auto simulation = std::make_unique<continue_node<continue_msg>>(analysis_graph, 
    [&](const continue_msg&){
      simulation_graph.wait_for_all();
  });

  auto scenario = std::make_unique<continue_node<continue_msg>>(analysis_graph, 
    [&](const continue_msg&){
      scenario_graph.wait_for_all();
  });
  make_edge(*simulation, *scenario);

  for(size_t r=0; r<num_run; r++) {
    simulation->try_put(continue_msg());
    analysis_graph.wait_for_all();

    for(size_t i=0; i<NUM_TIMERS; i++) {
      slacks[i].clear();
      coefficients[i].clear();
    }
  }
}

