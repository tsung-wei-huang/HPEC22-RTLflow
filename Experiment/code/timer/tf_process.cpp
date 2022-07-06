#include <taskflow/taskflow.hpp>  // the only include you need 
#include <experimental/filesystem>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h> 
#include <vector>
#include <fstream>

constexpr int NUM_TIMERS {64};

float correlation_coefficient(std::vector<float> &x, std::vector<float>& y) {
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

struct ProcessFramework : public tf::Framework {
  // Ctor
  ProcessFramework() {
    correlation_matrix.resize(NUM_TIMERS);
    coefficients.resize(NUM_TIMERS);
    for(auto i=0; i<NUM_TIMERS; i++) {
      coefficients[i].resize(NUM_TIMERS);
    }
  }

  auto& process_task(std::experimental::filesystem::path path, int argc, const char* argv[]) {
    // The first argument is the name of the binary
    char** args = static_cast<char**>(malloc(sizeof(char*)*(argc+2)));
    args[0] = static_cast<char*>(malloc(sizeof(char)*path.native().length()+1));
    ::strcpy(args[0], path.native().c_str());

    for(auto i=0; i<argc ;i++) {
      args[i+1] = static_cast<char*>(malloc(sizeof(char)*strlen(argv[i])+1));        
      ::strcpy(args[i+1], argv[i]);
    }
   
    // argument array must terminate with nullptr  
    args[argc+1] = nullptr;

    process_tasks.emplace_back(emplace([=](){     
      if(auto pid = ::fork(); pid == -1) {
        std::puts("Error when forking process!");
      }
      else if(pid > 0) {
        int status;
        ::waitpid(pid, &status, 0);
 
        for(auto i=0; i<argc+1 ;i++) {
          free(args[i]);
        }
        free(args);
      }
      else {
        execve(path.native().data(), args, nullptr);
        printf("Oh something went wrong : %s\n", strerror(errno));
        _exit(EXIT_FAILURE);
      }
    }));
    return process_tasks.back();
  }

  void build_task_graph() {
    std::vector<tf::Task> tasks;
    for(int i=0; i<NUM_TIMERS; i+=4) {
      tasks.emplace_back(emplace([&, i=i](){
        // Reduction every 4 files
        for(int j=i; j<i+4; j++) {
          std::string name {"output_" + std::to_string(j)};
          std::ifstream ifs(name);
          for(std::string line; std::getline(ifs, line);) {
            if(line.find("Slack") != std::string::npos) {
              line.erase(0, 7);
              correlation_matrix[j].push_back(std::stof(line));
            }
          }
          ifs.close();
        }
      }));
      tasks.back().gather(process_tasks[i], process_tasks[i+1], process_tasks[i+2], process_tasks[i+3])
           .name("Reduce_" + std::to_string(i));
    }
    auto sync = emplace([](){}).gather(tasks);
    
    tasks.clear();
    for(size_t i=0; i<NUM_TIMERS; i++) {
      sync.precede(emplace([&, i=i](){
        for(auto j=0; j<NUM_TIMERS; j++) { 
          if(i != j) {
            coefficients[i][j] = correlation_coefficient(correlation_matrix[i], correlation_matrix[j]);     
          }
        }
      }));
    }
   
  }

  std::vector<tf::Task> process_tasks;
  std::vector<std::vector<float>> correlation_matrix;
  std::vector<std::vector<float>> coefficients; 
};

int main(int argc, const char* argv[]){

  tf::Taskflow tf;
  ProcessFramework f;

  constexpr size_t num_args {2};
  const char* args[num_args];

  for(auto i=0; i<NUM_TIMERS; i++) {
    std::string sdc = "sdc_" + std::to_string(i+1);
    std::string ofile = "output_" + std::to_string(i);

    args[0] = sdc.data();   
    args[1] = ofile.data();
    f.process_task("/home/clin99/Software/OpenTimer/SC19/timer", num_args, args).name("OT_" + std::to_string(i));
  }
  f.build_task_graph();

  std::cout << f.dump() << std::endl;
  exit(0);

  tf.run_n(f, 1).get();

  //for(auto &v: f.correlation_matrix) {
  //  assert(v.size() == 100);
  //}
  for(auto &r : f.coefficients) {
   for(auto &c : r) {
     std::cout << c << ' ';
   }
   std::cout << '\n';
  }

  return 0;
}



