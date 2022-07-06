#include <ot/timer/timer.hpp>                     // top-level header to include 
#include <fstream>

int main(int argc, char *argv[]) {
  // argv:
  //  0: binary name 
  //  1: sdc name
  //  2: output file name

  //for(int i=0; i<argc; i++) {
  //  std::cout << i << " argv = " << argv[i] << std::endl;
  //}
  //exit(0);
  //exit(0);
  
  ot::Timer timer;                                // create a timer instance (thread-safe)
  
  std::string circuit_name = "tv80";
  std::experimental::filesystem::path path {"/home/clin99/Software/OpenTimer/example/"};

  auto sdc = path / circuit_name / "sdcs" / argv[1];

  path /= circuit_name;
  circuit_name.append(".v");
  auto verilog = path / circuit_name;

  timer.read_celllib("/home/clin99/Software/OpenTimer/example/tv80/tv80_Early.lib", ot::MIN) 
       .read_celllib("/home/clin99/Software/OpenTimer/example/tv80/tv80_Late.lib", ot::MAX) 
       .read_verilog(verilog)    
       .read_sdc(sdc)    
       .update_timing();    

  //if(auto tns = timer.report_tns(); tns) std::cout << "TNS: " << *tns << '\n';  // (O(N) action)
  //if(auto wns = timer.report_wns(); wns) std::cout << "WNS: " << *wns << '\n';  // (O(N) action)
  
  auto paths = timer.report_timing(100);
  assert(paths.size() == 100);
  
  //std::cout << "# critical paths: " << paths.size() << '\n';

  std::ofstream ofs(argv[2]);

  //for(size_t i=0; i<paths.size(); ++i) {
  for(size_t i=0; i<100; ++i) {

    //std::cout << "Path : " << i+1 << '\n'
    //          << "Slack: " << paths[i].slack << '\n'
    //          << "Split: " << (paths[i].endpoint->split() == ot::MIN ? "MIN" : "MAX") << '\n';

    ofs << "Path : " << i+1 << '\n'
        << "Slack: " << paths[i].slack << '\n'
        << "Split: " << (paths[i].endpoint->split() == ot::MIN ? "MIN" : "MAX") << '\n';


    for(const auto& point : paths[i]) {
      ofs << "  "
          << point.pin.name() << ' '
          << (point.transition == ot::RISE ? 'R' : 'F') << ' '
          << point.at;

      if(auto gate = point.pin.gate()) {
        ofs << " (" << gate->name() << ' ' << gate->cell_name() << ')';
      }
      ofs << '\n';
    }
    ofs << '\n';
  }

  //timer.dump_timer(std::cout);                    // dump the timer details (O(1) accessor)

  return 0;
}

