#pragma once 

#include <experimental/filesystem>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h> 
#include <vector>
#include <fstream>
#include <numeric>
#include <cmath>
#include <cassert>
#include <cstring>
#include <iomanip> 
#include <chrono>
#include <thread>
#include <iostream>

constexpr size_t NUM_ARGS {2};
constexpr int NUM_TIMERS {1024};
constexpr int BATCH_SIZE {8};
static_assert(NUM_TIMERS % BATCH_SIZE == 0);

inline float correlation_coefficient(std::vector<float> &x, std::vector<float>& y) {
  auto x_sum = std::accumulate(x.begin(), x.end(), 0);
  auto y_sum = std::accumulate(y.begin(), y.end(), 0);
  auto x_ssum = std::accumulate(x.begin(), x.end(), 0, [](auto l, auto r){ return l + r*r; });
  auto y_ssum = std::accumulate(y.begin(), y.end(), 0, [](auto l, auto r){ return l + r*r; });

  float xy_sum {0};
  for(size_t i=0; i<x.size(); i++) {
    xy_sum += x[i]*y[i];
  }

  return (x.size()*xy_sum - x_sum*y_sum)/
         std::sqrt((x.size()*x_ssum - x_sum*y_sum) * (x.size()*y_ssum - y_sum*y_sum));
}

inline char** create_args(int id, const std::experimental::filesystem::path& path) {
  std::string sdc = "sdc_" + std::to_string(id+1);
  std::string ofile = "output_" + std::to_string(id);

  // 1st arg: Binary name
  char** args = static_cast<char**>(malloc(sizeof(char*)*(NUM_ARGS+2)));
  args[0] = static_cast<char*>(malloc(sizeof(char)*path.native().length()+1));
  ::strcpy(args[0], path.native().c_str());

  // 2nd arg: SDC file name
  args[1] = static_cast<char*>(malloc(sizeof(char)*sdc.size()));
  ::strcpy(args[1], sdc.c_str());

  // 3rd arg: output file name
  args[2] = static_cast<char*>(malloc(sizeof(char)*ofile.size()));
  ::strcpy(args[2], ofile.c_str());

  // 4th arg: argument array must terminate with nullptr  
  args[3] = nullptr;

  return args;
}

inline void execute_process(const std::experimental::filesystem::path &path, char** args) {
  if(auto pid = ::fork(); pid == -1) {
    std::puts("Error when forking process!");
  }
  else if(pid > 0) {
    int status;
    ::waitpid(pid, &status, 0);

    free(args[0]);
    free(args[1]);
    free(args[2]);
    free(args);
  }
  else {
    execve(path.native().data(), args, nullptr);
    printf("Error during process execution : %s\n", strerror(errno));
    _exit(EXIT_FAILURE);
  }
}

inline void read_slack(int id, std::array<std::vector<float>, NUM_TIMERS>& slacks) {
  // Read slack for every BATCH_SIZE files
  for(int j=id; j<id+BATCH_SIZE; j++) {
    std::string name {"output_" + std::to_string(j)};
    {
      std::ifstream ifs(name);
      for(std::string line; std::getline(ifs, line);) {
        if(line.find("Slack") != std::string::npos) {
          line.erase(0, 7);
          slacks[j].push_back(std::stof(line));
        }
      }
      ifs.close();
    }
    std::experimental::filesystem::remove(name);
  }
}

void run_tbb(unsigned, unsigned=1);
void run_taskflow(unsigned, unsigned=1);

