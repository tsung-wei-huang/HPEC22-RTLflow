#include "process.hpp"
#include <tbb/task_scheduler_init.h>
#include <tbb/flow_graph.h>

using namespace tbb;
using namespace tbb::flow;

void run_tbb(unsigned num_threads, unsigned num_run){
  tbb::task_scheduler_init init(num_threads);

  std::array<std::vector<float>, NUM_TIMERS> slacks;
  std::array<std::vector<float>, NUM_TIMERS> coefficients;

  std::vector<std::unique_ptr<tbb::flow::graph>> process_graphs;
  std::vector<std::unique_ptr<continue_node<continue_msg>>> process_nodes;
  std::vector<std::unique_ptr<continue_node<continue_msg>>> file_nodes;

  const std::experimental::filesystem::path binary_path {"/home/clin99/Software/OpenTimer/SC19/timer"};
  constexpr int num_process {NUM_TIMERS/4};
  for(int i=0; i<num_process; i++) {
    process_graphs.emplace_back(std::make_unique<tbb::flow::graph>());
  }

  for(auto i=0; i<NUM_TIMERS; i++) {
    process_nodes.emplace_back(
      std::make_unique<continue_node<continue_msg>>(*process_graphs[i/4], 
        [&, id=i](const continue_msg&) { 
          auto args = create_args(id, binary_path);
          execute_process(binary_path, args);
        }
      )
    );
  }

  for(auto i=0; i<num_process; i++) {
    file_nodes.emplace_back(
      std::make_unique<continue_node<continue_msg>>(*process_graphs[i], 
        [&, id=i*4](const continue_msg&) { 
          read_slack(id, slacks);
        }
      )
    );
    make_edge(*process_nodes[i*4], *file_nodes.back());
    make_edge(*process_nodes[i*4+1], *file_nodes.back());
    make_edge(*process_nodes[i*4+2], *file_nodes.back());
    make_edge(*process_nodes[i*4+3], *file_nodes.back());
  }

  tbb::flow::graph simulation_graph;
  std::vector<std::unique_ptr<continue_node<continue_msg>>> simulation_nodes;
  for(int i=0; i<num_process; i++) {
    simulation_nodes.emplace_back(
      std::make_unique<continue_node<continue_msg>>(simulation_graph, 
        [&, id=i](const continue_msg&) { 
           for(int i=0; i<4; i++) {
             process_nodes[id*4 + i]->try_put(continue_msg());
           }
           process_graphs[id]->wait_for_all();
        }
      )
    );
  }

  tbb::flow::graph scenario_graph;
  std::vector<std::unique_ptr<continue_node<continue_msg>>> coeff_nodes;
  for(int i=0; i<NUM_TIMERS; i++) {
    coeff_nodes.emplace_back(std::make_unique<continue_node<continue_msg>>(scenario_graph, 
      [&, i=i](const continue_msg&){
        for(auto j=0; j<NUM_TIMERS; j++) { 
          if(i != j) {
            coefficients[i].push_back(correlation_coefficient(slacks[i], slacks[j]));
          }
          else {
            coefficients[i].push_back(0); 
          }
        }
    }));
  }

  tbb::flow::graph analysis_graph;
  auto simulation = std::make_unique<continue_node<continue_msg>>(analysis_graph, 
    [&](const continue_msg&){
      for(int i=0; i<num_process; i++) {
        simulation_nodes[i]->try_put(continue_msg());
      }
      simulation_graph.wait_for_all();
  });

  auto scenario = std::make_unique<continue_node<continue_msg>>(analysis_graph, 
    [&](const continue_msg&){
      for(int i=0; i<NUM_TIMERS; i++) {
        coeff_nodes[i]->try_put(continue_msg());
      }
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

