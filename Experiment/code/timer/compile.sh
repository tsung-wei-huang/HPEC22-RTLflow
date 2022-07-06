set -x 
#g++ -O2 -std=c++17 ./tbb.cpp -I ./tbb-2019_U4/include -L /home/clin99/Software/OpenTimer/SC19/tbb-2019_U4/build/linux_intel64_gcc_cc8_libc2.27_kernel4.18.0_release -lstdc++fs -ltbb -pthread 
#g++ -O2 -std=c++17 ./taskflow.cpp -I ../../cpp-taskflow -pthread -lstdc++fs 
g++ -g -O2 -std=c++17 -I ../ timer.cpp ../lib/libOpenTimer.a -o timer  -lstdc++fs -pthread  
